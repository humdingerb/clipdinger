/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Catalog.h>

#include "App.h"
#include "ClipListItem.h"
#include "ClipListView.h"
#include "Constants.h"
#include "MainWindow.h"

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
	fTarget.SendMessage(POPCLOSED);
}


ClipListView::ClipListView(const char* name)
	:
	BListView(BRect(), "name")
{
}


ClipListView::~ClipListView()
{
}


void
ClipListView::AttachedToWindow()
{
	BMessage message(ADJUSTCOLORS);
	fRunner	= new BMessageRunner(this, &message, kMinuteUnits * 60000000);
	BListView::AttachedToWindow();
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
		case POPCLOSED:
		{
			fShowingPopUpMenu = false;
			break;
		}
		case DELETE:
		{
			fShowingPopUpMenu = false;

			BMessenger messenger(my_app->fMainWindow);
			BMessage message(DELETE);
			messenger.SendMessage(&message);
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
ClipListView::AdjustColors()
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
		ClipListItem *sItem = dynamic_cast<ClipListItem *> (ItemAt(i));
		if (fade) {
			int32 minutes = (now - sItem->GetTimeAdded()) / 60;
			float level = B_NO_TINT + (0.2 / step * ((float)minutes / delay));
	//		printf("Clip: %s\n\tminutes: %i\t\tlevel: %f\n\n",
	//			fTitle.String(), minutes, level);
			sItem->SetColor(tint_color(ui_color(B_LIST_BACKGROUND_COLOR),
			(level < 1.2) ? level : 1.2));  // limit to 1.2
		} else
			sItem->SetColor(ui_color(B_LIST_BACKGROUND_COLOR));
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
		new BMessage(DELETE));
	menu->AddItem(item);

	menu->SetTargetForItems(this);
	menu->Go(screen, true, true, true);
	fShowingPopUpMenu = true;
}
