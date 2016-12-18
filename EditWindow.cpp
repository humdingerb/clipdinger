/*
 * Copyright 2015-2016. All rights reserved.
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


EditWindow::EditWindow(BRect frame, BString text)
	:
	BWindow(BRect(), B_TRANSLATE("Edit title"),
		B_MODAL_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS |
		B_CLOSE_ON_ESCAPE)
{
	fOriginalTitle = text;
	
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
			if (title != fOriginalTitle) {
				BMessenger messenger(my_app->fMainWindow);
				BMessage message(UPDATE_TITLE);
				message.AddString("edit_title", title);
				messenger.SendMessage(&message);
			}
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
	// Title
	fTitleControl = new BTextControl("title", B_TRANSLATE("Title:"),
		fOriginalTitle, NULL);
	fTitleControl->SetExplicitMinSize(BSize(250.0, B_SIZE_UNSET));

	// Info
	BFont infoFont(*be_plain_font);
	infoFont.SetFace(B_ITALIC_FACE);
	rgb_color infoColor = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_4_TINT);
	BTextView* infoText = new BTextView("tip", &infoFont, &infoColor,
		B_WILL_DRAW | B_SUPPORTS_LAYOUT);
	infoText->MakeEditable(false);
	infoText->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	infoText->SetStylable(true);
	infoText->SetAlignment(B_ALIGN_CENTER);
	infoText->SetText(B_TRANSLATE(
		"Leave title empty to return to original title."));

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
			.Add(infoText)
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
