/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "App.h"
#include "SettingsWindow.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SettingsWindow"


SettingsWindow::SettingsWindow(int32 limit, BRect frame)
	:
	BWindow(BRect(), B_TRANSLATE("Clipdinger settings"), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	originalLimit = limit;
	newLimit = limit;

	char string[4];
	snprintf(string, sizeof(string), "%d", originalLimit);
	
	fLimitBox = new BTextControl("limitfield", NULL, string,
		NULL);
	BStringView* limitlabel = new BStringView("limitlabel",
		B_TRANSLATE("entries in the clipboard history"));

	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(msgCANCEL));
	BButton* ok = new BButton("ok", B_TRANSLATE("OK"), new BMessage(msgOK));
	ok->MakeDefault(true);

	const float padding = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(padding)
		.AddGroup(B_HORIZONTAL)
			.Add(fLimitBox)
			.Add(limitlabel)
		.End()
		.AddStrut(padding)
		.AddGroup(B_HORIZONTAL)
			.Add(cancel)
			.Add(ok)
		.End();

	frame.OffsetBy(60.0, 60.0);
	MoveTo(frame.LeftTop());
}


SettingsWindow::~SettingsWindow()
{
	App *app = dynamic_cast<App *> (be_app);
	if (app->fMainWindow->Lock())
		app->fMainWindow->UpdatedSettings(newLimit);
	app->fMainWindow->Unlock();
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case msgCANCEL:
		{
			newLimit = originalLimit;
			Quit();
			break;
		}
		case msgOK:
		{
			newLimit = atoi(fLimitBox->Text());
			Quit();
			break;
		}
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}		
