/*
 * Copyright 2015-2022. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 *
 * A clipboard tool to manage clip history and favourites
 */

#include <AboutWindow.h>
#include <Catalog.h>
#include <Messenger.h>

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
	return true;
}


void
App::ReadyToRun()
{
	BRect rect = fSettings.GetWindowPosition();
	fMainWindow = new MainWindow(rect);
	fReplWindow = new ReplWindow(rect);
	fSettingsWindow = new SettingsWindow(rect);

	fMainWindow->MoveBy(10000, 0); // Move out of view to avoid flicker
	fMainWindow->Show();
	// fMainWindow->Minimize(true);
	fMainWindow->MoveBy(-10000, 0);
	if (fMainWindow->Lock()) {
		fMainWindow->fHistory->MakeFocus(true);
		fMainWindow->Unlock();
	}
}


void
App::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case ACTIVATE:
		{
			if (fMainWindow->Lock()) {
				fMainWindow->Minimize(false);
				fMainWindow->Activate(true);
				fMainWindow->Unlock();
			}
			break;
		}
		case CLIPMONITOR:
		{
			if (fReplWindow->Lock()) {
				if (fReplWindow->IsHidden())
					fReplWindow->Show();
				else
					fReplWindow->Activate();
				fReplWindow->Unlock();
			}
			break;
		}
		case SETTINGS:
		{
			if (fSettingsWindow->Lock()) {
				if (fSettingsWindow->IsHidden())
					fSettingsWindow->Show();
				else
					fSettingsWindow->Activate();
				fSettingsWindow->Unlock();
			}
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
	BAboutWindow* aboutW
		= new BAboutWindow(B_TRANSLATE_SYSTEM_NAME(kApplicationName), kApplicationSignature);
	aboutW->AddDescription(B_TRANSLATE(
		"Clipdinger provides a history of clippings of the system "
		"clipboard and lets you create favorites.\n\n"
		"Please let me know of any bugs you find or features you "
		"miss. Contact info is in the ReadMe, see menu item 'Help'."));
	aboutW->AddCopyright(2015, "Humdinger");
	aboutW->Show();
}


int
main()
{
	App app;
	app.Run();
	return 0;
}
