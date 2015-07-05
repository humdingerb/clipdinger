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
