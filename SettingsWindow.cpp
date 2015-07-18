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
	BWindow(BRect(), B_TRANSLATE("Clipdinger settings"),
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS)
{
	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		newLimit = originalLimit = settings->GetLimit();
		newFade = originalFade = settings->GetFade();
		newFadeDelay = originalFadeDelay = settings->GetFadeDelay();
		newFadeStep = originalFadeStep = settings->GetFadeStep();
		settings->Unlock();
	}

	_BuildLayout();

	char string[4];
	snprintf(string, sizeof(string), "%d", originalLimit);
	fLimitControl->SetText(string);
	fFadeBox->SetValue(originalFade);
	fDelaySlider->SetValue(originalFadeDelay);
	fStepSlider->SetValue(originalFadeStep);

	fDelaySlider->SetEnabled(originalFade);
	fStepSlider->SetEnabled(originalFade);
	
	frame.OffsetBy(60.0, 60.0);
	MoveTo(frame.LeftTop());
}


SettingsWindow::~SettingsWindow()
{
}


bool
SettingsWindow::QuitRequested()
{
	RevertSettings();
	UpdateSettings();

	return true;
}


void
SettingsWindow::RevertSettings()
{
	ClipdingerSettings* settings = my_app->Settings();

	if (settings->Lock()) {
		settings->SetLimit(originalLimit);
		settings->SetFade(originalFade);
		settings->SetFadeDelay(originalFadeDelay);
		settings->SetFadeStep(originalFadeStep);
		settings->Unlock();
	}
	return;
}


void
SettingsWindow::UpdateSettings()
{
	BMessenger messenger(my_app->fMainWindow);
	BMessage message(UPDATE_SETTINGS);
	message.AddInt32("limit", newLimit);
	messenger.SendMessage(&message);
	return;
}


void
SettingsWindow::UpdateFadeText()
{
	if (!newFade)
		fFadeText->SetText(B_TRANSLATE("Entries don't fade over time."));
	else {
		char min[4];
		char maxtint[4];
		char step[4];
		snprintf(min, sizeof(min), "%d", newFadeDelay * kMinuteUnits);
		snprintf(maxtint, sizeof(maxtint), "%d",
			newFadeStep * newFadeDelay * kMinuteUnits);
		snprintf(step, sizeof(step), "%d", newFadeStep);

		BString string(B_TRANSLATE(
		"Entries fade every %A% minutes.\n"
		"The maximal tint is reached after\n"
		"%B% minutes (in %C% steps)"));
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
	fDelaySlider = new BSlider(BRect(), "delay", B_TRANSLATE("Delay"),
		new BMessage(DELAY), 1, 12);	// 12 units á 5 minutes
	fDelaySlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fDelaySlider->SetHashMarkCount(12);
	fDelaySlider->SetKeyIncrementValue(1);
	fDelaySlider->SetModificationMessage(new BMessage(DELAY));
	fStepSlider = new BSlider(BRect(), "step", B_TRANSLATE("Steps"),
		new BMessage(STEP), 1, 10);
	fStepSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fStepSlider->SetHashMarkCount(10);
	fStepSlider->SetKeyIncrementValue(1);
	fStepSlider->SetModificationMessage(new BMessage(STEP));

	BFont infoFont(*be_plain_font);
	infoFont.SetFace(B_ITALIC_FACE);
	rgb_color infoColor = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_4_TINT);
	fFadeText = new BTextView("fadetext", &infoFont, &infoColor,
		B_WILL_DRAW | B_SUPPORTS_LAYOUT);
	fFadeText->MakeEditable(false);
	fFadeText->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fFadeText->SetStylable(true);
	fFadeText->SetAlignment(B_ALIGN_CENTER);
	UpdateFadeText();

	font_height fheight;
	infoFont.GetHeight(&fheight);
	fFadeText->SetExplicitMinSize(BSize(0.0,
		(fheight.ascent + fheight.descent + fheight.leading) * 3.0));

	// Buttons
	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(CANCEL));
	BButton* ok = new BButton("ok", B_TRANSLATE("OK"), new BMessage(OK));
	ok->MakeDefault(true);

	static const float spacing = be_control_look->DefaultItemSpacing();
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
				.Add(fDelaySlider)
			.End()
			.AddGroup(B_HORIZONTAL)
				.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
				.Add(fStepSlider)
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

			fDelaySlider->SetEnabled(newFade);
			fStepSlider->SetEnabled(newFade);
			break;
		}
		case DELAY:
		{
			newFadeDelay = fDelaySlider->Value();
			if (settings->Lock()) {
				settings->SetFadeDelay(newFadeDelay);
				settings->Unlock();
			}
			UpdateSettings();
			UpdateFadeText();
			break;
		}
		case STEP:
		{
			newFadeStep = fStepSlider->Value();
			if (settings->Lock()) {
				settings->SetFadeStep(newFadeStep);
				settings->Unlock();
			}
			printf("newFadeStep = %i\n", newFadeStep);
			UpdateSettings();
			UpdateFadeText();
			break;	
		}
		case CANCEL:
		{
			RevertSettings();
			UpdateSettings();
			Quit();
			break;
		}
		case OK:
		{
			newLimit = atoi(fLimitControl->Text());
			if (settings->Lock()) {
				settings->SetLimit(newLimit);
				settings->SetFade(newFade);
				settings->SetFadeDelay(newFadeDelay);
				settings->SetFadeStep(newFadeStep);
				settings->Unlock();
			}
			UpdateSettings();
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
