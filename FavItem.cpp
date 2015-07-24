/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <ControlLook.h>

#include <stdio.h>

#include "App.h"
#include "FavItem.h"
#include "Constants.h"


FavItem::FavItem(BString clip, int32 favnumber)
	:
	BListItem()
{
	fClip = clip;
	fFavNumber = favnumber;
}


FavItem::~FavItem()
{
}


void
FavItem::DrawItem(BView *view, BRect rect, bool complete)
{
	static const float spacing = be_control_look->DefaultLabelSpacing();

	// set background color
	rgb_color bgColor;

	if (IsSelected())
		bgColor = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
	else
		bgColor = ui_color(B_LIST_BACKGROUND_COLOR);

    view->SetHighColor(bgColor);
	view->SetLowColor(bgColor);
	view->FillRect(rect);

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
		BPoint(spacing * 3,
		rect.top + fheight.ascent + 3 + floorf(fheight.leading / 2)));

	// draw lines
	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_2_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
	view->StrokeLine(BPoint(kIconSize - 1 + spacing * 2, 0),
		BPoint(kIconSize - 1 + spacing * 2, rect.bottom));
}


void
FavItem::Update(BView* view, const BFont* finfo)
{
	// we need to DefaultLabelSpacing the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);

	static const float spacing = be_control_look->DefaultLabelSpacing();
	BString string(GetClip());
	view->TruncateString(&string, B_TRUNCATE_END, Width()- spacing * 4);
	SetTitle(string);

	font_height	fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent) + 5);
}
