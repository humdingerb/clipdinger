/*
 * Copyright 2015-2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 *
 * A clipboard tool to manage clip history and favourites
 */

#include <Catalog.h>

#include "App.h"
#include "Constants.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Application"

App::App()
	:
	BApplication(kApplicationSignature)
{
}


App::~App()
{
}


bool
App::QuitRequested()
{
	if (fSettingsWindow) {
		BMessenger messenger(fSettingsWindow);
		if (messenger.IsValid() && messenger.LockTarget())
			fSettingsWindow->Quit();
	}
	if (fReplWindow) {
		BMessenger messenger(fReplWindow);
		if (messenger.IsValid() && messenger.LockTarget())
			fReplWindow->Quit();
	}

	BApplication::QuitRequested();
}


void
App::ReadyToRun()
{
	BRect rect = fSettings.GetWindowPosition();
	fMainWindow = new MainWindow(rect);
	fReplWindow = new ReplWindow(rect);
	fSettingsWindow = new SettingsWindow(rect);

	fMainWindow->MoveBy(4000, 0); // Move out of view to avoid flicker
	fMainWindow->Show();
	fMainWindow->Minimize(true);
	fMainWindow->MoveBy(-4000, 0);
	if (fMainWindow->Lock())
		fMainWindow->fHistory->MakeFocus(true);
	fMainWindow->Unlock();
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case ACTIVATE:
		{
			if (fMainWindow->Lock())
				fMainWindow->Minimize(false);
			fMainWindow->Unlock();
			break;
		}
		case CLIPMONITOR:
		{
			if (fReplWindow->Lock()) {
				if (fReplWindow->IsHidden())
					fReplWindow->Show();
				else
					fReplWindow->Activate();
			}
			fReplWindow->Unlock();
			break;
		}
		case SETTINGS:
		{
			if (fSettingsWindow->Lock()) {
				if (fSettingsWindow->IsHidden())
					fSettingsWindow->Show();
				else
					fSettingsWindow->Activate();
			}
			fSettingsWindow->Unlock();
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
			break;
		}
	}
}


void
App::AboutRequested()
{
	BString text = B_TRANSLATE_COMMENT(
		"Clipdinger %version%\n"
		"\twritten by Humdinger\n"
		"\tCopyright %years%\n\n"
		"Clipdinger provides a history of clippings of the system "
		"clipboard and lets you create favorites.\n\n"
		"Please let me know of any bugs you find or features you "
		"miss. Contact info is in the ReadMe, see menu item 'Help'.",
		"Don't change the variables %years% and %version%.");
	text.ReplaceAll("%years%", kCopyright);
	text.ReplaceAll("%version%", kVersion);

	BAlert *alert = new BAlert("about", text.String(),
		B_TRANSLATE("Thank you"));

	BTextView *view = alert->TextView();
	BFont font;
	view->SetStylable(true);
	view->GetFont(&font);
	font.SetSize(font.Size() + 4);
	font.SetFace(B_BOLD_FACE);
	view->SetFontAndColor(0, 11, &font);
	alert->Go();
}


int
main()
{
	App app;
	app.Run();
	return 0;
}
