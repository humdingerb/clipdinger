/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPDINGERSETTINGS_H
#define CLIPDINGERSETTINGS_H

#include <Locker.h>
#include <Rect.h>


class ClipdingerSettings {
public:
					ClipdingerSettings();
					~ClipdingerSettings();

		bool		Lock();
		void		Unlock();
			
		int32		GetLimit() { return fLimit; }
		int32		GetAutoPaste() { return fAutoPaste; }
		int32		GetFade() { return fFade; }
		int32		GetFadeDelay() { return fFadeDelay; }
		int32		GetFadeStep() { return fFadeStep; }
		BRect		GetWindowPosition() { return fPosition; }
		void		GetSplitWeight(float* left, float* right);

		void		SetLimit(int32 limit) { fLimit = limit; }
		void		SetAutoPaste(int32 autopaste) { fAutoPaste = autopaste; }
		void		SetFade(int32 fade) { fFade = fade; }
		void		SetFadeDelay(int32 delay) { fFadeDelay = delay; }
		void		SetFadeStep(int32 step) { fFadeStep = step; }
		void		SetWindowPosition(BRect where) { fPosition = where; }
		void		SetSplitWeight(float left, float right);
private:
		int32		fLimit;
		int32		fAutoPaste;
		int32		fFade;
		int32		fFadeDelay;
		int32		fFadeStep;
		BRect		fPosition;
		float		fLeftWeight;
		float		fRightWeight;

		int32		originalLimit;
		int32		originalAutoPaste;
		int32		originalFade;
		int32		originalFadeDelay;
		int32		originalFadeStep;
		BRect		originalPosition;
		float		originalLeftWeight;
		float		originalRightWeight;
		
		BLocker		fLock;
};

#endif	/* CLIPDINGERSETTINGS_H */
