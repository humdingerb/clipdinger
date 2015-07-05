/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <ControlLook.h>

#include "ClipListItem.h"
#include "Constants.h"


ClipListItem::ClipListItem(BString clip)
	:
	BListItem()
{
	fTitle = new BString;
	fClip = new BString(clip);

	if (fClip->CountChars() > kMaxTitleChars) {
		fClip->CopyInto(*fTitle, 0, kMaxTitleChars);
		fTitle->Append(B_UTF8_ELLIPSIS);
	} else
		fTitle = fClip;
}


ClipListItem::~ClipListItem()
{
}


void
ClipListItem::DrawItem(BView *view, BRect rect, bool complete)
{
    // set background color
    if (IsSelected()) {
		rgb_color bgColor = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
		view->SetHighColor(bgColor);
		view->SetLowColor(bgColor);
    }
    else {
		rgb_color bgColor = ui_color(B_LIST_BACKGROUND_COLOR);
		view->SetHighColor(bgColor);
		view->SetLowColor(bgColor);
    }
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

	float width, height;
	view->GetPreferredSize(&width, &height);

    BString string(fTitle->String());
    view->TruncateString(&string, B_TRUNCATE_END, width -
		be_control_look->DefaultLabelSpacing() / 2);
    view->DrawString(string.String(),
		BPoint(rect.left + be_control_look->DefaultLabelSpacing(),
		rect.top + fheight.ascent + 2 + floorf(fheight.leading / 2)));

	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_1_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
}


void ClipListItem::Update(BView *view, const BFont *finfo)
{
	// we need to override the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);

	font_height	fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent) + 4);
}
