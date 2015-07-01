/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Rect.h>

static const char kSettingsFolder[] = "Clipdinger";
static const char kSettingsFile[] = "Clipdinger_settings";
const int32 kDefaultLimit = 50;

class Settings {
public:
					Settings();
					~Settings();

		int32		Limit() const { return fLimit; }
		BRect		WindowPosition() const { return fPosition; }
		void		SetLimit(int32 limit);
		void		SetWindowPosition(BRect where);
private:
		int32		fLimit;
		int32		originalLimit;
		BRect		fPosition;
		BRect		originalPosition;
};

#endif	/* SETTINGS_H */
