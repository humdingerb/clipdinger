/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include "Settings.h"

#include <TextControl.h>
#include <Window.h>

#define msgCANCEL			'cncl'
#define msgOK				'okay'


class SettingsWindow : public BWindow {
public:
					SettingsWindow(int32 limit, BRect frame);
	virtual			~SettingsWindow();

	void			MessageReceived(BMessage* message);		
private:
	BTextControl*	fLimitBox;
	int32			originalLimit;
	int32			newLimit;
};


#endif // SETTINGS_WINDOW_H
