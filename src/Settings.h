/*
 * Copyright 2015-2022. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef Settings_H
#define Settings_H

#include <Locker.h>
#include <Rect.h>

class Settings {
public:
					Settings();
					~Settings();

		bool		Lock();
		void		Unlock();

		void		LoadSettings();
		void		SaveSettings();

		int32		GetLimit() { return fLimit; }
		bool		GetAutoStart() { return fAutoStart; }
		bool		GetTrayIcon() { return fTrayIcon; }
		int32		GetAutoPaste() { return fAutoPaste; }
		int32		GetFade() { return fFade; }
		int32		GetFadeDelay() { return fFadeDelay; }
		int32		GetFadeStep() { return fFadeStep; }
		int32		GetFadeMaxLevel() { return fFadeMaxLevel; }
		int32		GetFadePause() { return fFadePause; }

		BRect		GetWindowPosition() { return fPosition; }
		void		GetSplitWeight(float& left, float& right);
		void		GetSplitCollapse(bool& left, bool& right);

		void		SetLimit(int32 limit);
		void		SetTrayIcon(int32 autostart);
		void		SetAutoStart(int32 autostart);
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
		bool		fTrayIcon;
		bool		fAutoStart;

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

		bool		dirtySettings;

		BLocker		fLock;
};

#endif	/* Settings_H */
