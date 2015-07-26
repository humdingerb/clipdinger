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
// #include "MainWindow.h"
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
FavView::FrameResized(float width, float height)
{
	BListView::FrameResized(width, height);
	
	static const float spacing = be_control_look->DefaultLabelSpacing();

	for (int32 i = 0; i < CountItems(); i++) {
		FavItem *sItem = dynamic_cast<FavItem *> (ItemAt(i));
		BString string(sItem->GetClip());
		TruncateString(&string, B_TRUNCATE_END, width - kIconSize
			- spacing * 4);
		sItem->SetDisplayTitle(string);
	}
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
FavView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case POPCLOSED:
		{
			fShowingPopUpMenu = false;
			break;
		}
		case FAV_DELETE:
		{
			fShowingPopUpMenu = false;
			Looper()->PostMessage(FAV_DELETE);
			break;
		}
		case FAV_EDIT:
		{
			fShowingPopUpMenu = false;
			FavItem *fav =
				dynamic_cast<FavItem *> (ItemAt(CurrentSelection()));
			fEditWindow = new EditWindow(Window()->Frame(), fav);
			fEditWindow->Show();
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
			Looper()->PostMessage(FAV_DELETE);
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
FavView::ShowPopUpMenu(BPoint screen)
{
	if (fShowingPopUpMenu)
		return;

	ContextPopUp* menu = new ContextPopUp("PopUpMenu", this);

	BMenuItem* item = new BMenuItem(B_TRANSLATE("Edit title"),
		new BMessage(FAV_EDIT));
	menu->AddItem(item);
	item = new BMenuItem(B_TRANSLATE("Remove favorite"),
		new BMessage(FAV_DELETE));
	menu->AddItem(item);

	menu->SetTargetForItems(this);
	menu->Go(screen, true, true, true);
	fShowingPopUpMenu = true;
}
