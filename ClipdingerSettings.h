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
		int32		GetFadeMaxLevel() { return fFadeMaxLevel; }
		int32		GetFadePause() { return fFadePause; }
		BRect		GetWindowPosition() { return fPosition; }
		void		GetSplitWeight(float* left, float* right);
		void		GetSplitCollapse(bool* left, bool* right);

		void		SetLimit(int32 limit);
		void		SetAutoPaste(int32 autopaste);
		void		SetFade(int32 fade);
		void		SetFadeDelay(int32 delay);
		void		SetFadeStep(int32 step);
		void		SetFadeMaxLevel(int32 level);
		void		SetWindowPosition(BRect where);
		void		SetSplitWeight(float left, float right);
		void		SetSplitCollapse(bool left, bool right);
		void		SetFadePause(int32 pause) { fFadePause = pause; }
private:
		int32		fLimit;
		int32		fAutoPaste;
		int32		fFade;
		int32		fFadeDelay;
		int32		fFadeStep;
		int32		fFadeMaxLevel;
		int32		fFadePause;
		BRect		fPosition;
		float		fLeftWeight;
		float		fRightWeight;
		bool		fLeftCollapse;
		bool		fRightCollapse;

		bool		dirtySettings;;
		
		BLocker		fLock;
};

#endif	/* CLIPDINGERSETTINGS_H */
