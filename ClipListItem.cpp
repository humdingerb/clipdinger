/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <ControlLook.h>
#include <Node.h>
#include <NodeInfo.h>

#include <stdio.h>

#include "App.h"
#include "ClipListItem.h"
#include "Constants.h"


ClipListItem::ClipListItem(BString clip, BString path, int32 time)
	:
	BListItem()
{
	fClip = clip; 
	fOrigin = path;
	fTimeAdded = time;

	BNode node;
	BNodeInfo node_info;

	if ((node.SetTo(path.String()) == B_NO_ERROR) &&
			(node_info.SetTo(&node) == B_NO_ERROR)) {
		fOriginIcon = new BBitmap(BRect(0, 0, kIconSize - 1, kIconSize - 1),
			0, B_RGBA32);
		status_t status = node_info.GetTrackerIcon(fOriginIcon, B_MINI_ICON);
		if (status != B_OK)
			fOriginIcon = NULL;
	} else
		fOriginIcon = NULL;

	if (fClip.CountChars() > kMaxTitleChars) {
		fClip.CopyInto(fTitle, 0, kMaxTitleChars);
		fTitle.Append(B_UTF8_ELLIPSIS);
	} else
		fTitle = fClip;
}


ClipListItem::~ClipListItem()
{
	delete fOriginIcon;
}


void
ClipListItem::DrawItem(BView *view, BRect rect, bool complete)
{
	float spacing = be_control_look->DefaultLabelSpacing();

	// set background color
	rgb_color bgColor;

	if (IsSelected())
		bgColor = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
    else {
		App *app = dynamic_cast<App *> (be_app);
		int32 now(real_time_clock());
		int32 minutes = (now - fTimeAdded) / 60;
		float step = app->fMainWindow->fSettings.FadeStep();
		int32 speed = app->fMainWindow->fSettings.FadeSpeed();

		float level = B_NO_TINT + (step * ((float)minutes / speed));
			printf("Clip: %s\n\tminutes: %i\t\tlevel: %f\n\n",
				fTitle.String(), minutes, level);
		bgColor = tint_color(ui_color(B_LIST_BACKGROUND_COLOR),
			(level < 1.2) ? level : 1.2);  // limit to 1.2
	}

    view->SetHighColor(bgColor);
	view->SetLowColor(bgColor);
	view->FillRect(rect);

	// icon of origin app
	if (fOriginIcon) {
        view->SetDrawingMode(B_OP_OVER);
        view->DrawBitmap(fOriginIcon, BPoint(rect.left + spacing,
			rect.top + (rect.Height() - kIconSize) / 2));
        view->SetDrawingMode(B_OP_COPY);
	} else
		printf("Found no icon\n");

	// text
	if (IsSelected())
    	view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
    else
    	view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

	BFont font;
	view->SetFont(&font);
	font_height	fheight;
	font.GetHeight(&fheight);

	float width, height;
	view->GetPreferredSize(&width, &height);

    BString string(fTitle.String());
    view->TruncateString(&string, B_TRUNCATE_END, width - kIconSize
		- spacing * 4);
    view->DrawString(string.String(),
		BPoint(kIconSize - 1 + spacing * 3,
		rect.top + fheight.ascent + 3 + floorf(fheight.leading / 2)));

	// draw lines
	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_2_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
	view->StrokeLine(BPoint(kIconSize - 1 + spacing * 2, 0),
		BPoint(kIconSize - 1 + spacing * 2, height));
}


void
ClipListItem::Update(BView *view, const BFont *finfo)
{
	// we need to DefaultLabelSpacing the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);

	font_height	fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent) + 5);
}


BString
ClipListItem::GetClip() { return fClip; };

BString
ClipListItem::GetOrigin() { return fOrigin; };

bigtime_t
ClipListItem::GetTimeAdded() { return fTimeAdded; };

void
ClipListItem::SetTimeAdded(int32 time) { fTimeAdded = time; };
