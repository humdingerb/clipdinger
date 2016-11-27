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
#include "FavItem.h"
#include "FavView.h"
#include "Constants.h"
#include "ContextPopUp.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ClipList"


FavView::FavView(const char* name)
	:
	BListView(name)
{
}


FavView::~FavView()
{
}


void
FavView::AttachedToWindow()
{
	SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE);

	BListView::AttachedToWindow();
}


void
FavView::Draw(BRect rect)
{
	SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));

	BRect bounds(Bounds());
	BRect itemFrame = ItemFrame(CountItems() - 1);
	bounds.top = itemFrame.bottom;
	FillRect(bounds);

	BListView::Draw(rect);
}


void
FavView::FrameResized(float width, float height)
{
	BListView::FrameResized(width, height);

	static const float spacing = be_control_look->DefaultLabelSpacing();

	for (int32 i = 0; i < CountItems(); i++) {
		FavItem *sItem = dynamic_cast<FavItem *> (ItemAt(i));
		BString string(sItem->GetTitle());
		TruncateString(&string, B_TRUNCATE_END, width - kIconSize
			- spacing * 4);
		sItem->SetDisplayTitle(string);
	}
}


void
FavView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case POPCLOSED:
		{
			fShowingPopUpMenu = false;
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
FavView::KeyDown(const char* bytes, int32 numBytes)
{
	switch (bytes[0]) {
		case B_DELETE:
		{
			FavItem* currentFav
				= dynamic_cast<FavItem *>(ItemAt(CurrentSelection()));
			BMessage message(FAV_DELETE);
			message.AddPointer("fav", currentFav);
			Looper()->PostMessage(&message);
			break;
		}
		case B_LEFT_ARROW:
		case B_RIGHT_ARROW:
		{
			BMessage message(SWITCHLIST);
			message.AddInt32("listview", (int32)1);
			Looper()->PostMessage(&message);
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
FavView::MouseDown(BPoint position)
{
	BListView::MouseDown(position);

	BMessage message(SWITCHLIST);
	message.AddInt32("listview", (int32)0);
	Looper()->PostMessage(&message);

	BRect bounds = ItemFrame(CurrentSelection());
	if (bounds.Contains(position)) {
		uint32 buttons = 0;
		if (Window() != NULL && Window()->CurrentMessage() != NULL)
			buttons = Window()->CurrentMessage()->FindInt32("buttons");

		if (buttons == B_SECONDARY_MOUSE_BUTTON)
			_ShowPopUpMenu(ConvertToScreen(position));
	}
}


// #pragma mark - Member Functions


void
FavView::_ShowPopUpMenu(BPoint screen)
{
	if (fShowingPopUpMenu)
		return;

	ContextPopUp* menu = new ContextPopUp("PopUpMenu", this);
	FavItem *currentFav = dynamic_cast<FavItem *>(ItemAt(CurrentSelection()));
	BMessage* msg = NULL;

	msg = new BMessage(FAV_EDIT);
	msg->AddPointer("fav", currentFav);
	BMenuItem* item = new BMenuItem(B_TRANSLATE("Edit title"), msg);
	menu->AddItem(item);

	msg = new BMessage(FAV_DELETE);
	msg->AddPointer("fav", currentFav);
	item = new BMenuItem(B_TRANSLATE("Remove favorite"),msg);
	menu->AddItem(item);

	msg = new BMessage(PASTE_SPRUNGE);
	msg->AddPointer("fav", currentFav);
	item = new BMenuItem(B_TRANSLATE("Paste to Sprunge.us"), msg);
	menu->AddItem(item);

	menu->SetTargetForItems(Looper());
	menu->Go(screen, true, true, true);
	fShowingPopUpMenu = true;
}
