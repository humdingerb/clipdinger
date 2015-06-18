/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 *
 * A clipboard tool to manage clip history and favourites
 */


#include "App.h"


extern const char *kApplicationSignature = "application/x-vnd.Clipdinger";


App::App()
	:
	BApplication(kApplicationSignature)
{	
	BRect aFrame;
	aFrame.Set(50.0, 50.0, 300.0, 400.0);
	fMainWindow = new MainWindow(aFrame);
	fMainWindow->Show();

	fPort = create_port(20, INPUT_PORT_NAME);
}


void
App::AppActivated(bool activated)
{
	fMainWindow->Minimize(false);
	printf("Activate! %d\n", activated);
}


App::~App()
{
}


void
App::AboutRequested()
{
	BAlert *alert = new BAlert("about", "Clipdinger v0.1\n"
		"\twritten by Humdinger\n"
		"\tCopyright 2015\n\n"
		"Clipdinger provides a history and manages favourites of "
		"clippings of the system clipboard.\n", "Thank you");
	BTextView *view = alert->TextView();
	BFont font;

	view->SetStylable(true);
	view->GetFont(&font);
	font.SetSize(font.Size()+4);
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
