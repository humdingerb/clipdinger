/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "App.h"
#include "SettingsWindow.h"

#include <Button.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

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
	fLimitBox->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);
	// only allow numbers
	for (uint32 i = 0; i < '0'; i++)
		fLimitBox->TextView()->DisallowChar(i);
	for (uint32 i = '9' + 1; i < 255; i++)
		fLimitBox->TextView()->DisallowChar(i);

	BStringView* limitlabel = new BStringView("limitlabel",
		B_TRANSLATE("entries in the clipboard history"));

	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(msgCANCEL));
	BButton* ok = new BButton("ok", B_TRANSLATE("OK"), new BMessage(msgOK));
	ok->MakeDefault(true);

	const float padding = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.SetInsets(padding)
			.Add(fLimitBox)
			.Add(limitlabel)
		.End()
		.AddStrut(padding)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.SetInsets(0, 0, 0, padding / 2)
			.AddGlue()
			.Add(cancel)
			.Add(ok)
			.AddGlue()
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
