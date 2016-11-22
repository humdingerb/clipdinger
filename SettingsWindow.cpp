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
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS |
		B_CLOSE_ON_ESCAPE)
{

	_BuildLayout();
	GetSettings();
	SetControls();

	frame.OffsetBy(260.0, 60.0);
	MoveTo(frame.LeftTop());
}


SettingsWindow::~SettingsWindow()
{
}


bool
SettingsWindow::QuitRequested()
{
	if (!IsHidden())
		Hide();
	return false;
}


void
SettingsWindow::GetSettings()
{
	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		newLimit = originalLimit = settings->GetLimit();
		newAutoStart = originalAutoStart = settings->GetAutoStart();
		newAutoPaste = originalAutoPaste = settings->GetAutoPaste();
		newFade = originalFade = settings->GetFade();
		newFadeDelay = originalFadeDelay = settings->GetFadeDelay();
		newFadeStep = originalFadeStep = settings->GetFadeStep();
		newFadeMaxLevel = originalFadeMaxLevel = settings->GetFadeMaxLevel();
		settings->Unlock();
	}
}


void
SettingsWindow::SetControls()
{
	char string[4];
	snprintf(string, sizeof(string), "%d", originalLimit);
	fLimitControl->SetText(string);
	fAutoStartBox->SetValue(originalAutoStart);
	fAutoPasteBox->SetValue(originalAutoPaste);
	fFadeBox->SetValue(originalFade);
	fDelaySlider->SetValue(originalFadeDelay);
	fStepSlider->SetValue(originalFadeStep);
	fLevelSlider->SetValue(originalFadeMaxLevel);

	fDelaySlider->SetEnabled(originalFade);
	fStepSlider->SetEnabled(originalFade);
	fLevelSlider->SetEnabled(originalFade);
}


void
SettingsWindow::RevertSettings()
{
	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		settings->SetLimit(originalLimit);
		settings->SetAutoStart(originalAutoStart);
		settings->SetAutoPaste(originalAutoPaste);
		settings->SetFade(originalFade);
		settings->SetFadeDelay(originalFadeDelay);
		settings->SetFadeStep(originalFadeStep);
		settings->SetFadeMaxLevel(originalFadeMaxLevel);
		settings->Unlock();
	}
	newLimit = originalLimit;
	newAutoStart = originalAutoStart;
	newAutoPaste = originalAutoPaste;
	newFade = originalFade;
	newFadeDelay = originalFadeDelay;
	newFadeStep = originalFadeStep;
	newFadeMaxLevel = originalFadeMaxLevel;
}


void
SettingsWindow::UpdateSettings()
{
	BMessenger messenger(my_app->fMainWindow);
	BMessage message(UPDATE_SETTINGS);
	message.AddInt32("limit", newLimit);
	message.AddInt32("autopaste", newAutoPaste);
	message.AddInt32("fade", newFade);
	messenger.SendMessage(&message);
}


void
SettingsWindow::UpdateFadeText()
{
	BString string;

	if (!newFade) {
		string = B_TRANSLATE("Entries don't fade over time.");
		string.Prepend("\n");
		string.Append("\n");
	} else {
		char min[4];
		char maxtint[4];
		char step[4];
		snprintf(min, sizeof(min), "%d", newFadeDelay * kMinuteUnits);
		snprintf(maxtint, sizeof(maxtint), "%d",
			newFadeStep * newFadeDelay * kMinuteUnits);
		snprintf(step, sizeof(step), "%d", newFadeStep);

		string = B_TRANSLATE(
		"Entries fade every %A% minutes.\n"
		"The maximal tint is reached after\n"
		"%B% minutes (in %C% steps)");
		string.ReplaceAll("%A%", min);
		string.ReplaceAll("%B%", maxtint);
		string.ReplaceAll("%C%", step);
	}
	fFadeText->SetText(string.String());
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

	// Auto-start
	fAutoStartBox = new BCheckBox("autostart", B_TRANSLATE(
		"Auto-start Clipdinger"), new BMessage(AUTOSTART));

	// Auto-paste
	fAutoPasteBox = new BCheckBox("autopaste", B_TRANSLATE(
		"Auto-paste"), new BMessage(AUTOPASTE));

	// Fading
	fFadeBox = new BCheckBox("fading", B_TRANSLATE(
		"Fade history entries over time"), new BMessage(FADE));
	fDelaySlider = new BSlider(BRect(), "delay", B_TRANSLATE("Delay"),
		new BMessage(DELAY), 1, 12);	// 12 units á 10 minutes
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

	fLevelSlider = new BSlider(BRect(), "level", B_TRANSLATE("Max. tint level"),
		new BMessage(LEVEL), 3, 14);
	fLevelSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fLevelSlider->SetHashMarkCount(12);
	fLevelSlider->SetKeyIncrementValue(1);
	fLevelSlider->SetModificationMessage(new BMessage(LEVEL));

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
	fFadeText->SetExplicitMinSize(BSize(300.0,
		(fheight.ascent + fheight.descent + fheight.leading) * 3.0));

	// Buttons
	BButton* cancel = new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(CANCEL));
	BButton* ok = new BButton("ok", B_TRANSLATE("OK"), new BMessage(OK));
	ok->MakeDefault(true);

	static const float spacing = be_control_look->DefaultItemSpacing();
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.SetInsets(spacing, spacing, spacing, 0)
			.Add(fLimitControl)
			.Add(limitlabel)
			.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing * 4))
		.End()
		.AddGroup(B_VERTICAL, 0)
			.SetInsets(spacing, 0, spacing, 0)
			.Add(fAutoStartBox)
			.Add(fAutoPasteBox)
			.End()
		.AddGroup(B_VERTICAL, spacing)
			.SetInsets(spacing, 0, spacing, 0)
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
				.Add(fLevelSlider)
			.End()
			.AddStrut(spacing / 2)
			.AddGroup(B_HORIZONTAL)
				.Add(BSpaceLayoutItem::CreateHorizontalStrut(spacing))
				.Add(fFadeText)
			.End()
		.End()
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL)
			.SetInsets(0, 0, 0, spacing)
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
		case AUTOSTART:
		{
			newAutoStart = fAutoStartBox->Value();
			UpdateSettings();
			break;
		}
		case AUTOPASTE:
		{
			newAutoPaste = fAutoPasteBox->Value();
			UpdateSettings();
			break;
		}
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
			fLevelSlider->SetEnabled(newFade);
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
			UpdateSettings();
			UpdateFadeText();
			break;
		}
		case LEVEL:
		{
			newFadeMaxLevel = fLevelSlider->Value();
			if (settings->Lock()) {
				settings->SetFadeMaxLevel(newFadeMaxLevel);
				settings->Unlock();
			}
			UpdateSettings();
			break;
		}
		case CANCEL:
		{
			RevertSettings();
			UpdateSettings();
			SetControls();
			Hide();
			break;
		}
		case OK:
		{
			newLimit = atoi(fLimitControl->Text());
			if (settings->Lock()) {
				settings->SetLimit(newLimit);
				settings->SetAutoStart(newAutoStart);
				settings->SetAutoPaste(newAutoPaste);
				settings->SetFade(newFade);
				settings->SetFadeDelay(newFadeDelay);
				settings->SetFadeStep(newFadeStep);
				settings->Unlock();
			}
			UpdateSettings();
			GetSettings();
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
