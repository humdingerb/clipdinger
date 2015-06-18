/*
 * Copyright 2014. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Application.h>

#include "ClipListItem.h"


ClipListItem::ClipListItem(BString clip)
		  :BListItem()
{
	printf("new item: %s\n", clip.String());
	fClip = clip;
}


ClipListItem::~ClipListItem()
{
}


void
ClipListItem::DrawItem(BView *view, BRect rect, bool complete)
{
    float offset = 10;

    // set background color
    if (IsSelected()) {
        view->SetHighColor(ui_color(B_LIST_SELECTED_BACKGROUND_COLOR));
        view->SetLowColor(ui_color(B_LIST_SELECTED_BACKGROUND_COLOR));    	
    }
    else {
        view->SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
        view->SetLowColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
    }
    view->FillRect(rect);

	// clip
    if (IsSelected())
    	view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
    else
    	view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

	BFont font = be_plain_font;
	font_height	finfo;
	font.GetHeight(&finfo);
    view->SetFont(&font);

	fFontHeight = rect.top + 12.8;

printf("finfo.ascent: %f\n", finfo.ascent);
printf("finfo.descent: %f\n", finfo.descent);
printf("fFontHeight: %f\n", fFontHeight);

	view->MovePenTo(offset, fFontHeight);

    float width, height;
    view->GetPreferredSize(&width, &height);

    BString string(fClip);
    view->TruncateString(&string, B_TRUNCATE_END, width - kBitmapSize - offset/2);
    view->DrawString(string.String());
}


void ClipListItem::Update(BView *owner, const BFont *finfo)
{
	// we need to override the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(owner, finfo);
	SetHeight(fFontHeight * 2);
}
