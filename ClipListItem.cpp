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
	fTitle = fClip;
	fOrigin = path;
	fTimeAdded = time;
	fColor = ui_color(B_LIST_BACKGROUND_COLOR);

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
	if (IsSelected())
    	view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
    else
    	view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

	BFont font;
	view->SetFont(&font);
	font_height	fheight;
	font.GetHeight(&fheight);

    view->DrawString(fTitle.String(),
		BPoint(kIconSize - 1 + spacing * 3,
		rect.top + fheight.ascent + 3 + floorf(fheight.leading / 2)));

	// draw lines
	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_2_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
	view->StrokeLine(BPoint(kIconSize - 1 + spacing * 2, 0),
		BPoint(kIconSize - 1 + spacing * 2, rect.bottom));
}


void
ClipListItem::Update(BView* view, const BFont* finfo)
{
	// we need to DefaultLabelSpacing the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);

	font_height	fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent) + 5);
}
