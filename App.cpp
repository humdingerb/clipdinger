/*
 * Copyright 2015. All rights reserved.
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
	BMessenger messenger(fMainWindow);
	if (messenger.IsValid() && messenger.LockTarget())
		fMainWindow->Quit();
}


void
App::ReadyToRun()
{
	fMainWindow = new MainWindow(BRect(fSettings.GetWindowPosition()));

	fMainWindow->MoveBy(4000, 0); // Move out of view to avoid flicker
	fMainWindow->Show();
	fMainWindow->Minimize(true);
	fMainWindow->MoveBy(-4000, 0);
}


void
App::AboutRequested()
{
	BAlert *alert = new BAlert("about",
		B_TRANSLATE("Clipdinger v0.5.2\n"
		"\twritten by Humdinger\n"
		"\tCopyright 2015-2016\n\n"
		"Clipdinger provides a history of clippings of the system "
		"clipboard and lets you create favorites.\n\n"
		"Please let me know of any bugs you find or features you "
		"miss. Contact info is in the ReadMe, see menu item 'Help'."),
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
