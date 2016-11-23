/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Button.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#include "App.h"
#include "Constants.h"
#include "EditWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "EditWindow"


EditWindow::EditWindow(BRect frame, FavItem* fav)
	:
	BWindow(BRect(), B_TRANSLATE("Edit title"),
		B_MODAL_WINDOW,
		B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS |
		B_CLOSE_ON_ESCAPE)
{
	fItem = fav;
	fOriginalTitle = fav->GetTitle();
	
	_BuildLayout();

	fTitleControl->MakeFocus(true);
	frame.OffsetBy(180.0, 80.0);
	MoveTo(frame.LeftTop());
}


EditWindow::~EditWindow()
{
}


void
EditWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
	case CANCEL:
		{
			Quit();
			break;
		}
		case OK:
		{
			BString title = fTitleControl->Text();
			fItem->SetTitle(title);
			fItem->SetDisplayTitle(title);

			BMessenger messenger(my_app->fMainWindow);
			BMessage message(UPDATE_FAV_DISPLAY);
			messenger.SendMessage(&message);

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


void
EditWindow::_BuildLayout()
{
	// Limit
	fTitleControl = new BTextControl("title", B_TRANSLATE("Title:"),
		fOriginalTitle, NULL);
	fTitleControl->SetExplicitMinSize(BSize(250.0, B_SIZE_UNSET));

	// Buttons
	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(CANCEL));
	BButton* ok = new BButton("ok", B_TRANSLATE("OK"), new BMessage(OK));
	ok->MakeDefault(true);

	static const float spacing = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_VERTICAL)
			.SetInsets(spacing)
			.Add(fTitleControl)
			.AddStrut(spacing / 2)
		.End()
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.SetInsets(0, 0, 0, spacing / 2)
			.AddGlue()
			.Add(cancel)
			.Add(ok)
			.AddGlue()
		.End();
}
