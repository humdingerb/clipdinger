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


ClipItem::ClipItem(BString clip, BString title, BString path, bigtime_t added, bigtime_t since)
	:
	BListItem(),
	fUpdateNeeded(true)
{
	fClip = clip;
	fTitle = title;

	if (title == "")
		fDisplayTitle = fClip;
	else
		fDisplayTitle = fTitle;

	fOrigin = path;
	fTimeAdded = added;
	fTimeSince = since;
	fColor = ui_color(B_LIST_BACKGROUND_COLOR);

	fIconSize = (int32(be_control_look->ComposeIconSize(16).Height()) + 1);
	BEntry entry(path.String());

	if (entry.InitCheck() == B_OK) {
		fOriginIcon = new BBitmap(BRect(0, 0, fIconSize - 1, fIconSize - 1), 0, B_RGBA32);
		entry_ref ref;
		entry.GetRef(&ref);
		BNodeInfo::GetTrackerIcon(&ref, fOriginIcon, icon_size(fIconSize));
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
		view->DrawBitmap(
			fOriginIcon, BPoint(rect.left + spacing, rect.top + (rect.Height() - fIconSize) / 2));
		view->SetDrawingMode(B_OP_COPY);
	} else
		printf("Found no icon\n");

	// text
	if (IsSelected() && my_app->fMainWindow->GetHistoryActiveFlag())
		view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
	else
		view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

	if (fUpdateNeeded) {
		BString title(GetTitle());
		view->TruncateString(&title, B_TRUNCATE_END, Width() - fIconSize - spacing * 4);
		fDisplayTitle = title;
		fUpdateNeeded = false;
	}

	BFont font;
	view->SetFont(&font);
	font_height fheight;
	font.GetHeight(&fheight);

	view->DrawString(fDisplayTitle.String(),
		BPoint(fIconSize - 1 + spacing * 3,
			rect.top + fheight.ascent + fheight.descent + fheight.leading));

	// draw lines
	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
	view->StrokeLine(BPoint(fIconSize - 1 + spacing * 2, rect.top),
		BPoint(fIconSize - 1 + spacing * 2, rect.bottom));
}


void
ClipItem::Update(BView* view, const BFont* finfo)
{
	// we need to DefaultLabelSpacing the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(view, finfo);

	static const float spacing = be_control_look->DefaultLabelSpacing();
	BString title(GetTitle());
	view->TruncateString(&title, B_TRUNCATE_END, Width() - fIconSize - spacing * 4);
	fDisplayTitle = title;

	font_height fheight;
	finfo->GetHeight(&fheight);

	SetHeight(ceilf(fheight.ascent + 2 + fheight.leading / 2 + fheight.descent) + 5);
}


BString
ClipItem::GetTitle()
{
	return ((fTitle == "") ? fClip : fTitle);
}


void
ClipItem::SetTitle(BString title, bool update)
{
	if (title == fClip)
		fTitle = "";
	else
		fTitle = (title == "") ? fClip : title;

	fUpdateNeeded = update;
}
