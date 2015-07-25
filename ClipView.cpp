/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Catalog.h>
#include <ControlLook.h>

#include "App.h"
#include "ClipItem.h"
#include "ClipView.h"
#include "Constants.h"
#include "MainWindow.h"
#include "ContextPopUp.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ClipList"


ClipView::ClipView(const char* name)
	:
	BListView("name")
{
}


ClipView::~ClipView()
{
}


void
ClipView::AttachedToWindow()
{
	SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE);
	SetEventMask(B_KEYBOARD_EVENTS);

	BMessage message(ADJUSTCOLORS);
	fRunner	= new BMessageRunner(this, &message, kMinuteUnits * 60000000);

	BListView::AttachedToWindow();
}


void
ClipView::FrameResized(float width, float height)
{
	BListView::FrameResized(width, height);
	
	static const float spacing = be_control_look->DefaultLabelSpacing();

	for (int32 i = 0; i < CountItems(); i++) {
		ClipItem *sItem = dynamic_cast<ClipItem *> (ItemAt(i));
		BString string(sItem->GetClip());
		TruncateString(&string, B_TRUNCATE_END, width - kIconSize
			- spacing * 4);
		sItem->SetTitle(string);
	}
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


void
ClipView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case POPCLOSED:
		{
			fShowingPopUpMenu = false;
			break;
		}
		case FAV_ADD:
		{
			fShowingPopUpMenu = false;
			Looper()->PostMessage(FAV_ADD);
			break;
		}
		case DELETE:
		{
			fShowingPopUpMenu = false;
			Looper()->PostMessage(DELETE);
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
	static const int32 kModifiers = B_SHIFT_KEY | B_COMMAND_KEY;

	if (strcasecmp(bytes, "v") == 0
			&& (modifiers() & kModifiers) == kModifiers) {
		Window()->Minimize(false);
		Window()->Activate(true);
	}
	else if (Window()->IsActive()) {
		switch (bytes[0]) {
			case B_DELETE:
			{
				Looper()->PostMessage(DELETE);
				break;
			}
			case B_ESCAPE:
			{
				Looper()->PostMessage(ESCAPE);
				break;
			}
			default:
			{
				BListView::KeyDown(bytes, numBytes);
				break;
			}
		}
	}
}


void
ClipView::AdjustColors()
{
	bool fade;
	int32 delay;
	float step;
	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		fade = settings->GetFade();
		step = settings->GetFadeStep();
		delay = settings->GetFadeDelay() * kMinuteUnits;
		settings->Unlock();
	}

	int32 now(real_time_clock());
	for (int32 i = 0; i < CountItems(); i++) {
		ClipItem *sItem = dynamic_cast<ClipItem *> (ItemAt(i));
		if (fade) {
			int32 minutes = (now - sItem->GetTimeAdded()) / 60;
			float level = B_NO_TINT + (1.2 / step * ((float)minutes / delay));
			sItem->SetColor(tint_color(ui_color(B_LIST_BACKGROUND_COLOR),
			(level < 1.2) ? level : 1.2));  // limit to 1.2
		} else
			sItem->SetColor(ui_color(B_LIST_BACKGROUND_COLOR));
	}
}


void
ClipView::MouseDown(BPoint position)
{
	BRect bounds(Bounds());
	BRect itemFrame = ItemFrame(CountItems() - 1);
	bounds.top = itemFrame.bottom;
	if (bounds.Contains(position) == false) {
		uint32 buttons = 0;
		if (Window() != NULL && Window()->CurrentMessage() != NULL)
			buttons = Window()->CurrentMessage()->FindInt32("buttons");

		if (buttons == B_SECONDARY_MOUSE_BUTTON)
			ShowPopUpMenu(ConvertToScreen(position));
	}
	BListView::MouseDown(position);
}


void
ClipView::ShowPopUpMenu(BPoint screen)
{
	if (fShowingPopUpMenu)
		return;

	ContextPopUp* menu = new ContextPopUp("PopUpMenu", this);

	BMenuItem* item = new BMenuItem(B_TRANSLATE("Remove clip"),
		new BMessage(DELETE));
	menu->AddItem(item);

	item = new BMenuItem(B_TRANSLATE("Add to favorites"),
		new BMessage(FAV_ADD));
	menu->AddItem(item);

	menu->SetTargetForItems(this);
	menu->Go(screen, true, true, true);
	fShowingPopUpMenu = true;
}
