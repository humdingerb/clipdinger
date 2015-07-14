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
#include <SpaceLayoutItem.h>

#include "App.h"
#include "Constants.h"
#include "SettingsWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SettingsWindow"


SettingsWindow::SettingsWindow(BRect frame)
	:
	BWindow(BRect(), B_TRANSLATE("Clipdinger settings"), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		newLimit = originalLimit = settings->GetLimit();
		newFade = originalFade = settings->GetFade();
		newFadeSpeed = originalFadeSpeed = settings->GetFadeSpeed();
		newFadeStep = originalFadeStep = settings->GetFadeStep();
		settings->Unlock();
	}

	_BuildLayout();

	char string[4];
	snprintf(string, sizeof(string), "%d", originalLimit);
	fLimitControl->SetText(string);
	fFadeBox->SetValue(originalFade);
	fFadeSpeed->SetValue(originalFadeSpeed);
	fFadeStep->SetValue(originalFadeStep * 100);

	fFadeSpeed->SetEnabled(originalFade);
	fFadeStep->SetEnabled(originalFade);
	
	frame.OffsetBy(60.0, 60.0);
	MoveTo(frame.LeftTop());
}


SettingsWindow::~SettingsWindow()
{
}


bool
SettingsWindow::QuitRequested()
{
	UpdateSettings();

	this->Hide();
	return false;
}


void
SettingsWindow::UpdateSettings()
{
	my_app->fMainWindow->UpdatedSettings(newLimit);
}


void
SettingsWindow::UpdateFadeText()
{
	if (!newFade)
		fFadeText->SetText(B_TRANSLATE("Entries don't fade over time."));
	else {
		BString string(B_TRANSLATE("Entries fade every %A% minutes.\n"
		"The maximal tint is reached after %B% minutes (%C% steps)"));
		char min[4];
		char maxtint[4];
		char step[4];
		snprintf(min, sizeof(min), "%d", newFadeSpeed);
		snprintf(maxtint, sizeof(maxtint), "%d",
			floor((0.2 / newFadeStep * newFadeSpeed) + 0.5));
		snprintf(step, sizeof(step), "%d",
			floor((0.2 / newFadeStep) + 0.5));
		string.ReplaceAll("%A%", min);
		string.ReplaceAll("%B%", maxtint);
		string.ReplaceAll("%C%", step);

		fFadeText->SetText(string.String());
	}
	return;
}

void
SettingsWindow::_BuildLayout()
{
	// Limit
	fLimitControl = new BTextControl("limitfield", NULL, "",
		NULL);
	fLimitControl->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);
	// only allow numbers
	for (uint32 i = 0; i < '0'; i++)
		fLimitControl->TextView()->DisallowChar(i);
	for (uint32 i = '9' + 1; i < 255; i++)
		fLimitControl->TextView()->DisallowChar(i);

	BStringView* limitlabel = new BStringView("limitlabel",
		B_TRANSLATE("entries in the clipboard history"));

	// Fading
	fFadeBox = new BCheckBox("fading", B_TRANSLATE(
		"Fade history entries over time"), new BMessage(FADE));
	fFadeSpeed = new BSlider(BRect(), "speed", B_TRANSLATE("Speed"),
		new BMessage(SPEED), 1, 60);
	fFadeStep = new BSlider(BRect(), "step", B_TRANSLATE("Stepsize"),
		new BMessage(STEP), 1, 10);

	BFont infoFont(*be_plain_font);
	infoFont.SetFace(B_ITALIC_FACE);
	rgb_color infoColor = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_4_TINT);
	BRect textBounds = Bounds();
	fFadeText = new BTextView(textBounds, "fadetext", textBounds, &infoFont,
		&infoColor, B_FOLLOW_NONE, B_WILL_DRAW | B_SUPPORTS_LAYOUT);
	fFadeText->MakeEditable(false);
	fFadeText->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fFadeText->SetStylable(true);
	fFadeText->SetAlignment(B_ALIGN_CENTER);
	UpdateFadeText();

	// Buttons
	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(CANCEL));
	BButton* ok = new BButton("ok", B_TRANSLATE("OK"), new BMessage(OK));
	ok->MakeDefault(true);

	const float spacing = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.SetInsets(spacing)
			.Add(fLimitControl)
			.Add(limitlabel)
		.End()
		.AddGroup(B_VERTICAL)
			.SetInsets(spacing, 0, spacing, spacing)
			.Add(fFadeBox)
			.AddGroup(B_HORIZONTAL)
				.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
				.Add(fFadeSpeed)
			.End()
			.AddGroup(B_HORIZONTAL)
				.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
				.Add(fFadeStep)
			.End()
			.AddGroup(B_HORIZONTAL)
				.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
				.Add(fFadeText)
			.End()
		.End()
		.AddStrut(spacing)
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.SetInsets(0, 0, 0, spacing / 2)
			.AddGlue()
			.Add(cancel)
			.Add(ok)
			.AddGlue()
		.End();
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	ClipdingerSettings* settings = my_app->Settings();

	switch (message->what)
	{
		case FADE:
		{
			newFade = fFadeBox->Value();
			if (settings->Lock()) {
				settings->SetFade(newFade);
				settings->Unlock();
			}
			UpdateSettings();
			UpdateFadeText();

			fFadeSpeed->SetEnabled(newFade);
			fFadeStep->SetEnabled(newFade);
			break;
		}
		case SPEED:
		{
			newFadeSpeed = fFadeSpeed->Value();
			if (settings->Lock()) {
				settings->SetFadeSpeed(newFadeSpeed);
				settings->Unlock();
			}
			UpdateSettings();
			UpdateFadeText();
			break;
		}
		case STEP:
		{
			newFadeStep = fFadeStep->Value();
			if (settings->Lock()) {
				settings->SetFadeStep(newFadeStep / 100);
				settings->Unlock();
			}
			printf("newFadeStep = %f\n", newFadeStep / 100);
			UpdateSettings();
			UpdateFadeText();
			break;	
		}
		case CANCEL:
		{
			if (settings->Lock()) {
				settings->SetLimit(originalLimit);
				settings->SetFade(originalFade);
				settings->SetFadeSpeed(originalFadeSpeed);
				settings->SetFadeStep(originalFadeStep / 100);
				settings->Unlock();
			}
			UpdateSettings();
			Hide();
			break;
		}
		case OK:
		{
			newLimit = atoi(fLimitControl->Text());
			if (settings->Lock()) {
				settings->SetLimit(newLimit);
				settings->SetFade(newFade);
				settings->SetFadeSpeed(newFadeSpeed);
				settings->SetFadeStep(newFadeStep / 100);
				settings->Unlock();
			}
			UpdateSettings();
			Hide();
			break;
		}
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}		
