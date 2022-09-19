/*
 * Copyright 2015-2022. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Button.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <Deskbar.h>
#include <LayoutBuilder.h>
#include <Roster.h>
#include <SeparatorView.h>
#include <SpaceLayoutItem.h>
#include <StringFormat.h>

#include "App.h"
#include "Constants.h"
#include "SettingsWindow.h"

#include <stdio.h>
#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SettingsWindow"


SettingsWindow::SettingsWindow(BRect frame)
	:
	BWindow(BRect(), B_TRANSLATE("Clipdinger settings"),
		B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS,
		B_ALL_WORKSPACES)
{
	_BuildLayout();
	_GetSettings();
	_UpdateControls();

	frame.OffsetBy(260.0, 60.0);
	MoveTo(frame.LeftTop());

	if (newTrayIcon)
		_AddIconToDeskbar();
}


SettingsWindow::~SettingsWindow()
{
	_RemoveIconFromDeskbar();
}


void
SettingsWindow::DispatchMessage(BMessage* message, BHandler* handler)
{
	if (_KeyDown(message) == B_OK)	// Was ESC pressed?
		return;

	BWindow::DispatchMessage(message, handler);
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	Settings* settings = my_app->GetSettings();

	switch (message->what)
	{
		case TRAYICON:
		{
			newTrayIcon = fTrayIconBox->Value();
			if (newTrayIcon)
				_AddIconToDeskbar();
			else
				_RemoveIconFromDeskbar();
			break;
		}
		case AUTOSTART:
		{
			newAutoStart = fAutoStartBox->Value();
			_UpdateMainWindow();
			break;
		}
		case AUTOPASTE:
		{
			newAutoPaste = fAutoPasteBox->Value();
			_UpdateMainWindow();
			break;
		}
		case FADE:
		{
			newFade = fFadeBox->Value();
			if (settings->Lock()) {
				settings->SetFade(newFade);
				settings->Unlock();
			}
			_UpdateMainWindow();
			_UpdateFadeText();

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
			_UpdateMainWindow();
			_UpdateFadeText();
			break;
		}
		case STEP:
		{
			newFadeStep = fStepSlider->Value();
			if (settings->Lock()) {
				settings->SetFadeStep(newFadeStep);
				settings->Unlock();
			}
			_UpdateMainWindow();
			_UpdateFadeText();
			break;
		}
		case LEVEL:
		{
			newFadeMaxLevel = fLevelSlider->Value();
			if (settings->Lock()) {
				settings->SetFadeMaxLevel(newFadeMaxLevel);
				settings->Unlock();
			}
			_UpdateMainWindow();
			break;
		}
		case CANCEL:
		{
			_RevertSettings();
			_UpdateMainWindow();
			_UpdateControls();
			if (!IsHidden())
				Hide();
			break;
		}
		case OK:
		{
			newLimit = atoi(fLimitControl->Text());
			if (newLimit == 0) {
				newLimit = 1;	// History has to be at least 1 deep.
				fLimitControl->SetText("1");
			}

			if (settings->Lock()) {
				settings->SetLimit(newLimit);
				settings->SetTrayIcon(newTrayIcon);
				settings->SetAutoStart(newAutoStart);
				settings->SetAutoPaste(newAutoPaste);
				settings->SetFade(newFade);
				settings->SetFadeDelay(newFadeDelay);
				settings->SetFadeStep(newFadeStep);
				settings->Unlock();
			}
			_UpdateMainWindow();
			_GetSettings();
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


bool
SettingsWindow::QuitRequested()
{
	if (!IsHidden())
		Hide();
	return false;
}


// #pragma mark - Layout & Display


status_t
SettingsWindow::_KeyDown(BMessage* message)
{
	uint32 raw_char  = message->FindInt32("raw_char");
	switch (raw_char)
	{
		case B_ESCAPE:
		{
			PostMessage(CANCEL);
			return B_OK;
		}
	}
	return B_ERROR;
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

	// Tray icon
	fTrayIconBox = new BCheckBox("trayicon", B_TRANSLATE(
		"Show icon in Deskbar tray"), new BMessage(TRAYICON));

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
		new BMessage(DELAY), 1, 12);	// 12 units á kMinuteUnits minutes
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
			.Add(fTrayIconBox)
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
SettingsWindow::_AddIconToDeskbar()
{
	app_info appInfo;
	be_app->GetAppInfo(&appInfo);

	BDeskbar deskbar;

	if (!deskbar.IsRunning())
		return;

	if (deskbar.HasItem(kApplicationName))
		_RemoveIconFromDeskbar();	// there could be a leftover replicant from a crashed Clipdinger

	status_t res = deskbar.AddItem(&appInfo.ref);
	if (res != B_OK)
		printf("Failed adding deskbar icon: %" B_PRId32 "\n", res);
}


void
SettingsWindow::_RemoveIconFromDeskbar()
{
	BDeskbar deskbar;
	int32 found_id;

	if (deskbar.GetItemInfo(kApplicationName, &found_id) == B_OK) {
		status_t err = deskbar.RemoveItem(found_id);
		if (err != B_OK) {
			printf("Clipdinger: Error removing replicant id "
				"%" B_PRId32 ": %s\n", found_id, strerror(err));
		}
	}
}


void
SettingsWindow::_UpdateControls()
{
	char string[4];
	snprintf(string, sizeof(string), "%" B_PRId32, originalLimit);
	fLimitControl->SetText(string);
	fTrayIconBox->SetValue(originalTrayIcon);
	if (originalTrayIcon)
		_AddIconToDeskbar();
	else
		_RemoveIconFromDeskbar();

	fAutoStartBox->SetValue(originalAutoStart);
	fAutoPasteBox->SetValue(originalAutoPaste);
	fFadeBox->SetValue(originalFade);
	fDelaySlider->SetValue(originalFadeDelay);
	fStepSlider->SetValue(originalFadeStep);
	fLevelSlider->SetValue(originalFadeMaxLevel);

	fDelaySlider->SetEnabled(originalFade);
	fStepSlider->SetEnabled(originalFade);
	fLevelSlider->SetEnabled(originalFade);

	_UpdateFadeText();
}


void
SettingsWindow::_UpdateFadeText()
{
	BString fadeText;

	if (!newFade) {
		fadeText = B_TRANSLATE("Entries don't fade over time.");
		fadeText.Prepend("\n");
		fadeText.Append("\n");
	} else {
		char min[5];
		char maxtint[5];
		snprintf(min, sizeof(min), "%" B_PRId32, newFadeDelay * kMinuteUnits);
		snprintf(maxtint, sizeof(maxtint), "%" B_PRId32,
			newFadeStep * newFadeDelay * kMinuteUnits);

		static BStringFormat fadeFormat(B_TRANSLATE("{0, plural,"
			"=1{Entries fade every %A% minutes.\n"
				"The maximal tint is reached after\n"
				"%B% minutes (in 1 step)}"
			"other{Entries fade every %A% minutes.\n"
				"The maximal tint is reached after\n"
				"%B% minutes (in # steps)}}"));
		fadeFormat.Format(fadeText, newFadeStep);

		fadeText.ReplaceAll("%A%", min);
		fadeText.ReplaceAll("%B%", maxtint);
	}
	fFadeText->SetText(fadeText.String());
}


void
SettingsWindow::_UpdateMainWindow()
{
	BMessenger messenger(my_app->fMainWindow);
	BMessage message(UPDATE_SETTINGS);
	message.AddInt32("limit", newLimit);
	message.AddInt32("autopaste", newAutoPaste);
	message.AddInt32("fade", newFade);
	messenger.SendMessage(&message);
}


// #pragma mark - Settings


void
SettingsWindow::_GetSettings()
{
	Settings* settings = my_app->GetSettings();
	if (settings->Lock()) {
		newLimit = originalLimit = settings->GetLimit();
		newTrayIcon = originalTrayIcon = settings->GetTrayIcon();
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
SettingsWindow::_RevertSettings()
{
	Settings* settings = my_app->GetSettings();
	if (settings->Lock()) {
		settings->SetLimit(originalLimit);
		settings->SetTrayIcon(originalTrayIcon);
		settings->SetAutoStart(originalAutoStart);
		settings->SetAutoPaste(originalAutoPaste);
		settings->SetFade(originalFade);
		settings->SetFadeDelay(originalFadeDelay);
		settings->SetFadeStep(originalFadeStep);
		settings->SetFadeMaxLevel(originalFadeMaxLevel);
		settings->Unlock();
	}
	newLimit = originalLimit;
	newAutoStart = originalTrayIcon;
	newAutoStart = originalAutoStart;
	newAutoPaste = originalAutoPaste;
	newFade = originalFade;
	newFadeDelay = originalFadeDelay;
	newFadeStep = originalFadeStep;
	newFadeMaxLevel = originalFadeMaxLevel;
}
