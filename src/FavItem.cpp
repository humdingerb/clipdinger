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


FavItem::FavItem(BString clip, BString title, int32 favnumber)
	:
	BListItem(),
	fUpdateNeeded(true)
{
	fFavNumber = favnumber;
	fClip = clip;
	fTitle = title;

	if (title == "")
		fDisplayTitle = fClip;
	else
		fDisplayTitle = fTitle;
}


FavItem::~FavItem()
{
}


void
FavItem::DrawItem(BView* view, BRect rect, bool complete)
{
	static const float spacing = be_control_look->DefaultLabelSpacing();

	// set background color
	rgb_color bgColor;

	if (IsSelected() && !my_app->fMainWindow->GetHistoryActiveFlag())
		bgColor = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
	else
		bgColor = ui_color(B_LIST_BACKGROUND_COLOR);

	view->SetHighColor(bgColor);
	view->SetLowColor(bgColor);
	view->FillRect(rect);

	// text
	BFont font(be_plain_font);
	font.SetFace(B_BOLD_FACE);
	view->SetFont(&font);
	font_height	fheight;
	font.GetHeight(&fheight);

	BString Fn("F");
	if (fFavNumber < 9)
		Fn.Append("0");

	char string[12];
	snprintf(string, sizeof(string), "%" B_PRId32, fFavNumber + 1);
	Fn.Append(string);

	float Fnwidth = font.StringWidth(Fn.String());

	if (!IsSelected()
			|| (IsSelected() && my_app->fMainWindow->GetHistoryActiveFlag())) {
		BRect Fnrect(rect.LeftTop(),
			BPoint(spacing * 2 + Fnwidth, rect.bottom));
		view->SetHighColor(tint_color(ui_color(B_LIST_BACKGROUND_COLOR), 1.08));
		view->FillRect(Fnrect);
	}
	if (IsSelected() && !my_app->fMainWindow->GetHistoryActiveFlag())
		view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
	else
		view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

	if (fFavNumber < 12) {
		view->DrawString(Fn.String(), BPoint(spacing,
		rect.top + fheight.ascent + fheight.descent + fheight.leading));
	}

	if (fUpdateNeeded) {
		BString title(GetTitle());
		view->TruncateString(&title, B_TRUNCATE_END, Width() - spacing * 7);
		fDisplayTitle = title;
		fUpdateNeeded = false;
	}

	font.SetFace(B_REGULAR_FACE);
	view->SetFont(&font);

	view->DrawString(fDisplayTitle.String(), BPoint(spacing * 3 + Fnwidth,
		rect.top + fheight.ascent + fheight.descent + fheight.leading));

	// draw lines
	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_2_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
	view->StrokeLine(BPoint(spacing * 2 + Fnwidth, rect.top),
		BPoint(spacing * 2 + Fnwidth, rect.bottom));
}


void
FavItem::Update(BView* view, const BFont* finfo)
{
	// we need to DefaultLabelSpacing the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);

	static const float spacing = be_control_look->DefaultLabelSpacing();
	BString title(GetTitle());
	view->TruncateString(&title, B_TRUNCATE_END, Width() - spacing * 7);
	fDisplayTitle = title;
	font_height	fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent) + 5);
}


BString
FavItem::GetTitle()
{
	return ((fTitle == "") ? fClip : fTitle);
}


void
FavItem::SetTitle(BString title, bool update)
{
	if (title == fClip)
		fTitle = "";
	else
		fTitle = (title == "") ? fClip : title;

	fUpdateNeeded = update;
}
