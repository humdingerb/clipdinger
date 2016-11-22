/*
 * Copyright 2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <Size.h>
#include <TextView.h>

#include "Constants.h"
#include "ReplWindow.h"
#include "ReplView.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ReplWindow"


ReplWindow::ReplWindow(BRect frame)
	:
	BWindow(BRect(0, 0, 350, 100), B_TRANSLATE("Clipboard monitor"),
		B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS |
		B_CLOSE_ON_ESCAPE)
{
	_BuildLayout();

	frame.OffsetBy(40.0, 90.0);
	MoveTo(frame.LeftTop());
}


ReplWindow::~ReplWindow()
{
}


bool
ReplWindow::QuitRequested()
{
	return true;
}


void
ReplWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}


void
ReplWindow::FrameResized(float width, float height)
{
	fClipMonitorView->TruncateClip(width);

	BWindow::FrameResized(width, height);
}


void
ReplWindow::_BuildLayout()
{
	// Clipboard contents view
	fClipMonitorView = new ReplView();

	// Text
	BFont infoFont(*be_plain_font);
	infoFont.SetFace(B_ITALIC_FACE);
	rgb_color infoColor = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_4_TINT);
	BTextView* welcomeText = new BTextView("text", &infoFont, &infoColor,
		B_WILL_DRAW | B_SUPPORTS_LAYOUT);
	welcomeText->MakeEditable(false);
	welcomeText->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	welcomeText->SetStylable(true);
	welcomeText->SetAlignment(B_ALIGN_CENTER);
	welcomeText->SetText(B_TRANSLATE(
		"Resize the window to your needs and drag the replicant handle "
		"onto the Desktop.\nTry dropping a color e.g. from Icon-O-Matic."));

	font_height fheight;
	infoFont.GetHeight(&fheight);
	welcomeText->SetExplicitMinSize(BSize(B_SIZE_UNSET,
		(fheight.ascent + fheight.descent + fheight.leading) * 3.0));
	welcomeText->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	static const float spacing = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Group<>(this, B_VERTICAL, spacing)
		.SetInsets(spacing)
		.Add(fClipMonitorView)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.Add(welcomeText)
		.AddGlue(10)
		.End();
}


