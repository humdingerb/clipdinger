/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "App.h"
#include "ClipListView.h"
#include "MainWindow.h"

#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ClipList"


class PopUpMenu : public BPopUpMenu {
public:
					PopUpMenu(const char* name, BMessenger target);
	virtual 		~PopUpMenu();

private:
	BMessenger 		fTarget;
};


PopUpMenu::PopUpMenu(const char* name, BMessenger target)
	:
	BPopUpMenu(name, false, false),
	fTarget(target)
{
	SetAsyncAutoDestruct(true);
}


PopUpMenu::~PopUpMenu()
{
	fTarget.SendMessage(msgPOPCLOSED);
}


ClipListView::ClipListView(const char* name)
		  :
		  BListView(BRect(), "name", B_SINGLE_SELECTION_LIST, B_WILL_DRAW)
{
}


ClipListView::~ClipListView()
{
}


void
ClipListView::Draw(BRect rect)
{
		SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
		FillRect(rect);
		BListView::Draw(rect);
}


void
ClipListView::FrameResized(float w, float h)
{
	BListView::FrameResized(w, h);
	
	for (int32 i = 0; i < CountItems(); i++) {
		BListItem *item = ItemAt(i);
		item->Update(this, be_plain_font);
	}
	Invalidate();
}


void
ClipListView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case msgPOPCLOSED:
		{
			fShowingPopUpMenu = false;
			break;
		}
		case msgDELETE:
		{
			fShowingPopUpMenu = false;

			App *app = dynamic_cast<App *> (be_app);
			BMessenger msgr(app->fMainWindow);
			BMessage refMsg(msgDELETE);
			msgr.SendMessage(&refMsg);
			break;
		}
		default:
			BView::MessageReceived(message);
			break;
	}
}


void
ClipListView::MouseDown(BPoint position)
{
	uint32 buttons = 0;
	if (Window() != NULL && Window()->CurrentMessage() != NULL)
		buttons = Window()->CurrentMessage()->FindInt32("buttons");

	if (buttons == B_SECONDARY_MOUSE_BUTTON)
		ShowPopUpMenu(ConvertToScreen(position));

	BListView::MouseDown(position);
}


void
ClipListView::ShowPopUpMenu(BPoint screen)
{
	if (fShowingPopUpMenu)
		return;

	PopUpMenu* menu = new PopUpMenu("PopUpMenu", this);

	BMenuItem* item = new BMenuItem(B_TRANSLATE("Remove clip"),
		new BMessage(msgDELETE));
	menu->AddItem(item);

	menu->SetTargetForItems(this);
	menu->Go(screen, true, true, true);
	fShowingPopUpMenu = true;
}
