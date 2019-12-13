/*
 * Copyright 2015-2018. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Catalog.h>
#include <ControlLook.h>
#include <DateFormat.h>
#include <TimeFormat.h>
#include <ToolTip.h>

#include "App.h"
#include "ClipItem.h"
#include "ClipView.h"
#include "Constants.h"
#include "ContextPopUp.h"
#include "MainWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ClipList"


ClipView::ClipView(const char* name)
	:
	BListView(name)
{
}


ClipView::~ClipView()
{
}


void
ClipView::AttachedToWindow()
{
	SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE);

	BMessage message(ADJUSTCOLORS);
	fRunner	= new BMessageRunner(this, &message, kMinuteUnits * 60000000);

	BListView::AttachedToWindow();
}


void
ClipView::Draw(BRect rect)
{
	SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));

	BRect bounds(Bounds());
	BRect itemFrame = ItemFrame(CountItems() - 1);
	bounds.top = itemFrame.bottom;
	FillRect(bounds);

	BListView::Draw(rect);
}


bool
ClipView::InitiateDrag(BPoint point, int32 dragIndex, bool wasSelected)
{
	BPoint pt;
	uint32 buttons;
	GetMouse(&pt, &buttons);

	if ((buttons & B_SECONDARY_MOUSE_BUTTON) != 0)
		return false;

	ClipItem* sItem = dynamic_cast<ClipItem *> (ItemAt(CurrentSelection()));
	if (sItem == NULL) {
		// workaround for a timing problem (see Locale prefs)
		sItem = dynamic_cast<ClipItem *> (ItemAt(dragIndex));
		Select(dragIndex);
		if (sItem == NULL)
			return false;
	}
	BString string(sItem->GetClip());
	BMessage message(B_SIMPLE_DATA);
	message.AddData("text/plain", B_MIME_TYPE, string, string.Length());
	message.AddInt32("clipdinger_command", FAV_ADD);

	BRect dragRect(0.0f, 0.0f, Bounds().Width(), sItem->Height());
	BBitmap* dragBitmap = new BBitmap(dragRect, B_RGB32, true);
	if (dragBitmap->IsValid()) {
		BView* view = new BView(dragBitmap->Bounds(), "helper", B_FOLLOW_NONE,
			B_WILL_DRAW);
		dragBitmap->AddChild(view);
		dragBitmap->Lock();

		sItem->DrawItem(view, dragRect);
		view->SetHighColor(0, 0, 0, 255);
		view->StrokeRect(view->Bounds());
		view->Sync();

		dragBitmap->Unlock();
	} else {
		delete dragBitmap;
		dragBitmap = NULL;
	}

	if (dragBitmap != NULL)
		DragMessage(&message, dragBitmap, B_OP_ALPHA, BPoint(0.0, 0.0));
	else
		DragMessage(&message, dragRect.OffsetToCopy(point), this);

	return true;
}


void
ClipView::MakeFocus(bool focused)
{
	BListView::MakeFocus(focused);

	// only signal ClipView is focused when gaining focus
	if (focused)
		my_app->fMainWindow->SetHistoryActiveFlag(focused);

}


void
ClipView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case POPCLOSED:
		{
			fShowingPopUpMenu = false;
			break;
		}
		case ADJUSTCOLORS:
		{
			AdjustColors();
			Invalidate();
			break;
		}
		default:
		{
			BListView::MessageReceived(message);
			break;
		}
	}
}


void
ClipView::KeyDown(const char* bytes, int32 numBytes)
{
	switch (bytes[0]) {
		case B_DELETE:
		{
			BMessage message(DELETE);
			Looper()->PostMessage(&message);
			break;
		}
		case B_RIGHT_ARROW:
		{
			if (my_app->fMainWindow->Lock())
				my_app->fMainWindow->fFavorites->MakeFocus(true);
			my_app->fMainWindow->Unlock();
			break;
		}
		default:
		{
			BListView::KeyDown(bytes, numBytes);
			break;
		}
	}
}


void
ClipView::MouseDown(BPoint position)
{
	MakeFocus(true);

	BRect bounds(Bounds());
	BRect itemFrame = ItemFrame(CountItems() - 1);
	bounds.top = itemFrame.bottom;
	if (bounds.Contains(position))
		return;

	uint32 buttons = 0;
	if (Window() != NULL && Window()->CurrentMessage() != NULL)
		buttons = Window()->CurrentMessage()->FindInt32("buttons");

	if ((buttons & B_SECONDARY_MOUSE_BUTTON) != 0) {
		Select(IndexOf(position));
		_ShowPopUpMenu(ConvertToScreen(position));
		return;
	}
	BListView::MouseDown(position);
}


bool
ClipView::GetToolTipAt(BPoint point, BToolTip** _tip)
{
	ClipItem* item = static_cast<ClipItem*>(this->ItemAt(this->IndexOf(point)));
	if (item == NULL)
		return false;

	BString dateString = "";
	bigtime_t added = item->GetTimeAdded();
	if (BDateFormat().Format(dateString, added,
		B_MEDIUM_DATE_FORMAT) != B_OK)
		return false;

	BString timeString = "";
	added = item->GetTimeAdded();
	if (BTimeFormat().Format(timeString, added,
		B_SHORT_TIME_FORMAT) != B_OK)
		return false;

	BString clipString(item->GetClip());
	// Add ellipsis if text length is > 300 chars
	if (clipString.Length() > 300) {
		clipString.Truncate(300);
		clipString << B_UTF8_ELLIPSIS;
	}

	BString toolTip(B_TRANSLATE_COMMENT("Added: %time% %date%\n%clip%",
		"Tooltip, don't change the variables %time% and %date%."));
	toolTip.ReplaceAll("%time%", timeString.String());
	toolTip.ReplaceAll("%date%", dateString.String());
	toolTip.ReplaceAll("%clip%", clipString.String());

	SetToolTip(toolTip.String());
	*_tip = ToolTip();

	ToolTip()->View()->SetExplicitPreferredSize(BSize(300, B_SIZE_UNSET));
	if (BTextView* textView = dynamic_cast<BTextView*>(ToolTip()->View()))
		textView->SetWordWrap(true);
	return true;
}


// #pragma mark - Member Functions


void
ClipView::AdjustColors()
{
	bool fade = false;
	int32 delay = 0;
	int32 step = 0;
	float maxlevel = 0;
	bool pause = false;

	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		fade = settings->GetFade();
		step = settings->GetFadeStep();
		delay = settings->GetFadeDelay() * kMinuteUnits;
		maxlevel = 1.0 + 0.025 * settings->GetFadeMaxLevel();
		pause = settings->GetFadePause();
		settings->Unlock();
	}

	if (pause)
		return;

	bigtime_t now(real_time_clock());
	for (int32 i = 0; i < CountItems(); i++) {
		ClipItem* sItem = dynamic_cast<ClipItem *> (ItemAt(i));
		if (fade) {
			bigtime_t minutes = (now - sItem->GetTimeSince()) / 60;
			float level = B_NO_TINT
				+ (maxlevel / step * ((float)minutes / delay) / kMinuteUnits);
			sItem->SetColor(tint_color(ui_color(B_LIST_BACKGROUND_COLOR),
			(level < maxlevel) ? level : maxlevel));  // limit to maxlevel
		} else
			sItem->SetColor(ui_color(B_LIST_BACKGROUND_COLOR));
	}
}


void
ClipView::_ShowPopUpMenu(BPoint screen)
{
	if (fShowingPopUpMenu)
		return;

	ContextPopUp* menu = new ContextPopUp("PopUpMenu", this);
	BMessage* msg = NULL;

	msg = new BMessage(PASTE_SPRUNGE);
	BMenuItem* item = new BMenuItem(B_TRANSLATE("Paste online"), msg, 'P');
	menu->AddItem(item);

	msg = new BMessage(FAV_ADD);
	msg->AddInt32("clipdinger_command", FAV_ADD);
	item = new BMenuItem(B_TRANSLATE("Add to favorites"), msg, 'A');
	menu->AddItem(item);

	msg = new BMessage(EDIT_TITLE);
	item = new BMenuItem(B_TRANSLATE("Edit title"), msg, 'E');
	menu->AddItem(item);

	msg = new BMessage(DELETE);
	item = new BMenuItem(B_TRANSLATE("Remove"), msg);
	menu->AddItem(item);

	menu->SetTargetForItems(Looper());
	menu->Go(screen, true, true, true);
	fShowingPopUpMenu = true;
}
