/*
 * Copyright 2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Catalog.h>
#include <Clipboard.h>
#include <ControlLook.h>
#include <Dragger.h>
#include <Font.h>
#include <LayoutBuilder.h>
#include <Roster.h>
#include <Size.h>
#include <String.h>
#include <View.h>

#include "Constants.h"
#include "ReplView.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ClipboardMonitor"


ReplView::ReplView()
	:
	BView("Clipboard monitor", B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	fReplicated(false)
{
	fContentsView = new BStringView("contents", "");
	fContentsView->SetAlignment(B_ALIGN_CENTER);
	fContentsView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	_SetColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));

	// Dragger
	BRect rect(Bounds());
	rect.left = rect.right - 7;
	rect.top = rect.bottom - 7;
	BDragger* dragger = new BDragger(rect, this,
		B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	dragger->SetExplicitMinSize(BSize(7, 7));
	dragger->SetExplicitMaxSize(BSize(7, 7));
	dragger->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_BOTTOM));

	// Layout
	font_height fheight;
	fContentsView->GetFontHeight(&fheight);

	SetExplicitMinSize(BSize(10.0,
		(fheight.ascent + fheight.descent + fheight.leading) * 1.5));
	SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	static const float spacing = be_control_look->DefaultLabelSpacing();

	BLayoutBuilder::Group<>(this, B_HORIZONTAL)
		.SetInsets(spacing, spacing / 2, spacing, spacing / 2.5)
		.Add(fContentsView)
		.Add(dragger, 0.01)
		.End();
}


ReplView::ReplView(BMessage* archive)
	:
	BView(archive),
	fReplicated(true)
{
	fContentsView = dynamic_cast<BStringView *> (FindView("contents"));
	fContentsView->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
}


ReplView::~ReplView()
{
}


BArchivable*
ReplView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "Clipboard monitor"))
		return NULL;

	return new ReplView(data);
}


status_t
ReplView::Archive(BMessage* archive, bool deep) const
{
	BView::Archive(archive, deep);

	archive->AddString("add_on", kApplicationSignature);
	archive->AddString("class", "Clipboard monitor");
		
	archive->PrintToStream();

	return B_OK;
}


void
ReplView::AttachedToWindow()
{
	BView::AttachedToWindow();

	fCurrentClip = _GetClipboard().String();
	TruncateClip(Bounds().Width());

	status_t error = be_clipboard->StartWatching(this);

	fContentsView->AddFilter(new BMessageFilter(B_MOUSE_DOWN,
		&ReplView::_MessageFilter));
}


void
ReplView::MouseDown(BPoint point)
{
	BMessage* msg = Window()->CurrentMessage();
	_LaunchClipdinger(msg);
}


void
ReplView::MessageReceived(BMessage* msg)
{
	if (msg->WasDropped()) {
		rgb_color* color;
		ssize_t size;
		if (msg->FindData("RGBColor", B_RGB_COLOR_TYPE,
			(const void **)&color, &size) == B_OK) {
				_SetColor(*color);
				return;
		}
	}
	switch (msg->what)
	{
		case B_CLIPBOARD_CHANGED:
		{
			fCurrentClip = _GetClipboard().String();
			if (fCurrentClip.Length() == 0)
				break;

			TruncateClip(Bounds().Width());
			break;
		}
		case B_ABOUT_REQUESTED:
		{
			BAlert* alert = new BAlert("about",
				B_TRANSLATE("Clipdinger v0.5.5\n"
				"\twritten by Humdinger\n"
				"\tCopyright 2015-2016\n\n"
				"Clipdinger provides a history of clippings of the system "
				"clipboard and lets you create favorites.\n\n"
				"This is its clipboard monitor that can be replicated to "
				"the Desktop to always show its current contents."),
				B_TRANSLATE("Thank you"));
		
			BTextView* view = alert->TextView();
			BFont font;
			view->SetStylable(true);
			view->GetFont(&font);
			font.SetSize(font.Size() + 4);
			font.SetFace(B_BOLD_FACE);
			view->SetFontAndColor(0, 11, &font);
			alert->Go();
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}


// #pragma mark - Member Functions


void
ReplView::TruncateClip(float width)
{
	static const float spacing = be_control_look->DefaultLabelSpacing();
	BString string(fCurrentClip);
	TruncateString(&string, B_TRUNCATE_END, width - spacing * 8);
	fContentsView->SetText(string);
}


BString
ReplView::_GetClipboard()
{
	const char* text;
	ssize_t textLen;
	BMessage* clipboard = (BMessage *)NULL;

	if (be_clipboard->Lock()) {
		if ((clipboard = be_clipboard->Data()))
			clipboard->FindData("text/plain", B_MIME_TYPE,
				(const void **)&text, &textLen);
		be_clipboard->Unlock();
	}
	if (text == NULL) {
		text = B_TRANSLATE("-= Clipboard is empty =-");
		textLen = strlen(text);
	}
	BString clip(text, textLen);
	return clip;
}


void
ReplView::_LaunchClipdinger(BMessage* msg)
{
	uint32 buttons = msg->FindInt32("buttons");
	int32 clicks = msg->FindInt32("clicks");

	if (buttons == B_PRIMARY_MOUSE_BUTTON && clicks >= 2) {
		team_id team;
		team = be_roster->TeamFor(kApplicationSignature);
		if (team < 0) {
			be_roster->Launch(kApplicationSignature);
			while (be_roster->TeamFor(kApplicationSignature) < 0)
				snooze(100000);
		}
		BMessenger messenger(kApplicationSignature);
		BMessage message(ACTIVATE);
		messenger.SendMessage(&message);
	}
}


filter_result
ReplView::_MessageFilter(BMessage* msg, BHandler** target, BMessageFilter* filter)
{
	ReplView* view = (ReplView *)(*target);
	switch (msg->what)
	{
		case B_MOUSE_DOWN:
		{
			view->_LaunchClipdinger(msg);
			return B_SKIP_MESSAGE;
		}
		default:
			return B_DISPATCH_MESSAGE;
	}
}


void
ReplView::_SetColor(rgb_color color)
{
	SetViewColor(color);
	fContentsView->SetViewColor(color);

	float thresh = color.red + (color.green * 1.5f) + (color.blue * 0.50f);

	if (thresh >= 300) {
		color.red = 0;
		color.green = 0;
		color.blue = 0;
	} else {
		color.red = 255;
		color.green = 255;
		color.blue = 255;
	}
	fContentsView->SetHighColor(color);
	fContentsView->SetLowColor(color);
	fContentsView->Invalidate();
	Invalidate();
}
