/*
 * Copyright 2015. All rights reserved.
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

	void			MessageReceived(BMessage* message);
	bool			QuitRequested();
	void			_BuildLayout();
	void			UpdateFadeText();
	void			UpdateSettings();

private:
	BTextControl*	fLimitControl;
	BCheckBox*		fFadeBox;
	BSlider*		fFadeSpeed;
	BSlider*		fFadeStep;
	BTextView*		fFadeText;
	BString*		fFadeDescription;

	int32			originalLimit;
	int32			originalFade;
	int32			originalFadeSpeed;
	float			originalFadeStep;

	int32			newLimit;
	int32			newFade;
	int32			newFadeSpeed;
	float			newFadeStep;
};


#endif // SETTINGS_WINDOW_H
