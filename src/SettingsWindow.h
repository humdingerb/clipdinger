/*
 * Copyright 2015-2022. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <CheckBox.h>
#include <Slider.h>
#include <TextControl.h>
#include <TextView.h>
#include <Window.h>


class SettingsWindow : public BWindow {
public:
					SettingsWindow(BRect frame);
	virtual			~SettingsWindow();

	void			DispatchMessage(BMessage* message, BHandler* handler);
	void			MessageReceived(BMessage* message);
	bool			QuitRequested();

private:
	status_t		_KeyDown(BMessage* message);
	void			_BuildLayout();
	void			_AddIconToDeskbar();
	void			_RemoveIconFromDeskbar();
	void			_UpdateControls();
	void			_UpdateFadeText();
	void			_UpdateMainWindow();

	void			_GetSettings();
	void			_RevertSettings();

	BTextControl*	fLimitControl;
	BCheckBox*		fFadeBox;
	BCheckBox*		fTrayIconBox;
	BCheckBox*		fAutoStartBox;
	BCheckBox*		fAutoPasteBox;
	BSlider*		fDelaySlider;
	BSlider*		fStepSlider;
	BSlider*		fLevelSlider;
	BTextView*		fFadeText;
	BString*		fFadeDescription;

	int32			originalLimit;
	int32			originalTrayIcon;
	int32			originalAutoStart;
	int32			originalAutoPaste;
	int32			originalFade;
	int32			originalFadeDelay;
	int32			originalFadeStep;
	int32			originalFadeMaxLevel;

	int32			newLimit;
	int32			newTrayIcon;
	int32			newAutoStart;
	int32			newAutoPaste;
	int32			newFade;
	int32			newFadeDelay;
	int32			newFadeStep;
	int32			newFadeMaxLevel;
};

#endif // SETTINGS_WINDOW_H
