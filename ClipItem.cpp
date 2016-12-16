/*
 * Copyright 2015-2016. All rights reserved.
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
#include "ClipItem.h"
#include "Constants.h"


ClipItem::ClipItem(BString clip, BString title, BString path, bigtime_t added,
	bigtime_t since)
	:
	BListItem(),
	fUpdateNeeded(true)
{
	fClip = clip;
	if (title != "")
		fDisplayTitle = title;
	else if (fClip.CountChars() > kMaxTitleChars) {
		fClip.CopyInto(fDisplayTitle, 0, kMaxTitleChars);
		fDisplayTitle.Append(B_UTF8_ELLIPSIS);
	} else
		fDisplayTitle = fClip;

	fTitle = fDisplayTitle;

	fOrigin = path;
	fTimeAdded = added;
	fTimeSince = since;
	fColor = ui_color(B_LIST_BACKGROUND_COLOR);

	BNode node;
	BNodeInfo node_info;

	if ((node.SetTo(path.String()) == B_NO_ERROR)
			&& (node_info.SetTo(&node) == B_NO_ERROR)) {
		fOriginIcon = new BBitmap(BRect(0, 0, kIconSize - 1, kIconSize - 1),
			0, B_RGBA32);
		status_t status = node_info.GetTrackerIcon(fOriginIcon, B_MINI_ICON);
		if (status != B_OK)
			fOriginIcon = NULL;
	} else
		fOriginIcon = NULL;
}


ClipItem::~ClipItem()
{
	delete fOriginIcon;
}


void
ClipItem::DrawItem(BView* view, BRect rect, bool complete)
{
	static const float spacing = be_control_look->DefaultLabelSpacing();

	// set background color
	rgb_color bgColor;

	if (IsSelected() && my_app->fMainWindow->GetHistoryActiveFlag())
		bgColor = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
	else
		bgColor = fColor;

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
	if (IsSelected() && my_app->fMainWindow->GetHistoryActiveFlag())
    	view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
    else
    	view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

	if (fUpdateNeeded) {
		BString string(GetTitle());
//		float wide = Width();
		view->TruncateString(&string, B_TRUNCATE_END, Width()
			- kIconSize - spacing * 4);
		fDisplayTitle = string;
		fUpdateNeeded = false;
	}

	BFont font;
	view->SetFont(&font);
	font_height	fheight;
	font.GetHeight(&fheight);

    view->DrawString(fDisplayTitle.String(),
		BPoint(kIconSize - 1 + spacing * 3,
		rect.top + fheight.ascent + fheight.descent + fheight.leading));

	// draw lines
	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_2_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
	view->StrokeLine(BPoint(kIconSize - 1 + spacing * 2, rect.top),
		BPoint(kIconSize - 1 + spacing * 2, rect.bottom));
}


void
ClipItem::Update(BView* view, const BFont* finfo)
{
	// we need to DefaultLabelSpacing the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);
// printf("ClipItem::Update(): item %p, width %f\n", this, Width());

	static const float spacing = be_control_look->DefaultLabelSpacing();
	BString string = fTitle;
	view->TruncateString(&string, B_TRUNCATE_END, Width() - kIconSize
			- spacing * 4);
	fDisplayTitle = string;

	font_height	fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent) + 5);
}


void
ClipItem::SetDisplayTitle(BString display, bool update)
{
	fDisplayTitle = display;
	fUpdateNeeded = update;
}
