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

	delete fPortRunner;
}


void
App::ReadyToRun()
{
	fMainWindow = new MainWindow(fSettings.GetWindowPosition());
	fMainWindow->Show();
//	fMainWindow->Minimize(true);

	PostMessage('crea');
}


void
App::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case PORTQUEUE:
		{
			port_info info;
			get_port_info(fPort, &info);
			if (info.queue_count) {
				int32 code;
				read_port(fPort, &code, NULL, 0 );

				if (code == 'CtSV') {
					fMainWindow->Minimize(false);
					fMainWindow->Activate(true);
				}
			}
			break;
		}
		case 'crea':
		{
			fPort = create_port(20, INPUT_PORT_NAME);

			BMessage message(PORTQUEUE);
			fPortRunner = new BMessageRunner(this, &message, 100000);
			break;
		}
		default:
		{
			BApplication::MessageReceived(message);
			break;
		}
	}
}


void
App::AboutRequested()
{
	BAlert *alert = new BAlert("about",
		B_TRANSLATE("Clipdinger v0.2\n"
		"\twritten by Humdinger\n"
		"\tCopyright 2015\n\n"
		"Clipdinger provides a history of "
		"clippings of the system clipboard.\n"),
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
