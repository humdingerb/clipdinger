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

		int32		Limit();
		void		SetLimit(int32 limit);

		int32		FadeSpeed();
		void		SetFadeSpeed(int32 speed);

		float		FadeStep();
		void		SetFadeStep(float step);

		BRect		WindowPosition();
		void		SetWindowPosition(BRect where);
private:
		int32		fLimit;
		int32		fFadeSpeed;
		float		fFadeStep;
		BRect		fPosition;

		int32		originalLimit;
		int32		originalFadeSpeed;
		float		originalFadeStep;
		BRect		originalPosition;
};

#endif	/* SETTINGS_H */
