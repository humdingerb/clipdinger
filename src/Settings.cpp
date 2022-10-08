/*
 * Copyright 2015-2022. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Font.h>
#include <Message.h>
#include <Path.h>

#include <stdio.h>
#include <stdlib.h>

#include "Constants.h"
#include "Settings.h"


Settings::Settings()
	:
	fLimit(kDefaultLimit),
	fTrayIcon(kDefaultTrayIcon),
	fAutoStart(kDefaultAutoStart),
	fAutoPaste(kDefaultAutoPaste),
	fFade(kDefaultFade),
	fFadeDelay(kDefaultFadeDelay),
	fFadeStep(kDefaultFadeStep),
	fFadeMaxLevel(kDefaultFadeMaxLevel),
	fFadePause(0),
	fPosition(-1, -1, -1, -1),
	fLeftWeight(0.8),
	fRightWeight(0.2),
	fLeftCollapse(false),
	fRightCollapse(false),
	dirtySettings(false)

{
	LoadSettings();
}


Settings::~Settings()
{
	if (dirtySettings)
		SaveSettings();
}


void
Settings::LoadSettings()
{
	BPath path;
	BMessage msg;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(kSettingsFolder);
		if (ret == B_OK) {
			path.Append(kSettingsFile);
			BFile file(path.Path(), B_READ_ONLY);

			if ((file.InitCheck() == B_OK) && (msg.Unflatten(&file) == B_OK)) {
				if (msg.FindInt32("limit", &fLimit) != B_OK) {
					fLimit = kDefaultLimit;
					dirtySettings = true;
				}
				if (msg.FindBool("trayicon", &fTrayIcon) != B_OK) {
					fTrayIcon = kDefaultTrayIcon;
					dirtySettings = true;
				}
				if (msg.FindBool("autostart", &fAutoStart) != B_OK) {
					fAutoStart = kDefaultAutoStart;
					dirtySettings = true;
				}
				if (msg.FindInt32("autopaste", &fAutoPaste) != B_OK) {
					fAutoPaste = kDefaultAutoPaste;
					dirtySettings = true;
				}
				if (msg.FindInt32("fade", &fFade) != B_OK) {
					fFade = kDefaultFade;
					dirtySettings = true;
				}
				if (msg.FindInt32("fadedelay", &fFadeDelay) != B_OK) {
					fFadeDelay = kDefaultFadeDelay;
					dirtySettings = true;
				}
				if (msg.FindInt32("fadestep", &fFadeStep) != B_OK) {
					fFadeStep = kDefaultFadeStep;
					dirtySettings = true;
				}
				if (msg.FindInt32("fademax", &fFadeMaxLevel) != B_OK) {
					fFadeStep = kDefaultFadeMaxLevel;
					dirtySettings = true;
				}
				if (msg.FindRect("windowlocation", &fPosition) != B_OK)
					fPosition.Set(-1, -1, -1, -1);

				if (msg.FindFloat("split_weight_left", &fLeftWeight) != B_OK)
					fLeftWeight = 0.8;

				if (msg.FindFloat("split_weight_right", &fRightWeight) != B_OK)
					fRightWeight = 0.2;

				if (msg.FindBool("split_collapse_left", &fLeftCollapse) != B_OK)
					fLeftCollapse = false;

				if (msg.FindBool("split_collapse_right", &fRightCollapse) != B_OK)
					fRightCollapse = false;
			}
		}
	}
}


void
Settings::SaveSettings()
{
	BPath path;
	BMessage msg;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) < B_OK)
		return;
	status_t ret = path.Append(kSettingsFolder);

	if (ret == B_OK)
		ret = create_directory(path.Path(), 0777);

	if (ret == B_OK)
		path.Append(kSettingsFile);

	if (ret == B_OK) {
		BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE);
		ret = file.InitCheck();

		if (ret == B_OK) {
			msg.AddInt32("limit", fLimit);
			msg.AddBool("trayicon", fTrayIcon);
			msg.AddBool("autostart", fAutoStart);
			msg.AddInt32("autopaste", fAutoPaste);
			msg.AddInt32("fade", fFade);
			msg.AddInt32("fadedelay", fFadeDelay);
			msg.AddInt32("fadestep", fFadeStep);
			msg.AddInt32("fademax", fFadeMaxLevel);
			msg.AddRect("windowlocation", fPosition);
			msg.AddFloat("split_weight_left", fLeftWeight);
			msg.AddFloat("split_weight_right", fRightWeight);
			msg.AddBool("split_collapse_left", fLeftCollapse);
			msg.AddBool("split_collapse_right", fRightCollapse);
			msg.Flatten(&file);
		}
	}
}


bool
Settings::Lock()
{
	return fLock.Lock();
}


void
Settings::Unlock()
{
	fLock.Unlock();
}


void
Settings::GetSplitWeight(float& left, float& right)
{
	left = fLeftWeight;
	right = fRightWeight;
}


void
Settings::GetSplitCollapse(bool& left, bool& right)
{
	left = fLeftCollapse;
	right = fRightCollapse;
}


void
Settings::SetLimit(int32 limit)
{
	if (fLimit == limit)
		return;
	fLimit = limit;
	dirtySettings = true;
}


void
Settings::SetTrayIcon(int32 trayicon)
{
	if (fTrayIcon == trayicon)
		return;
	fTrayIcon = trayicon;
	dirtySettings = true;
}


void
Settings::SetAutoStart(int32 autostart)
{
	if (fAutoStart == autostart)
		return;
	fAutoStart = autostart;
	dirtySettings = true;
}


void
Settings::SetAutoPaste(int32 autopaste)
{
	if (fAutoPaste == autopaste)
		return;
	fAutoPaste = autopaste;
	dirtySettings = true;
}


void
Settings::SetFade(int32 fade)
{
	if (fFade == fade)
		return;
	fFade = fade;
	dirtySettings = true;
}


void
Settings::SetFadeDelay(int32 delay)
{
	if (fFadeDelay == delay)
		return;
	fFadeDelay = delay;
	dirtySettings = true;
}


void
Settings::SetFadeStep(int32 step)
{
	if (fFadeStep == step)
		return;
	fFadeStep = step;
	dirtySettings = true;
}


void
Settings::SetFadeMaxLevel(int32 level)
{
	if (fFadeMaxLevel == level)
		return;
	fFadeMaxLevel = level;
	dirtySettings = true;
}


void
Settings::SetWindowPosition(BRect where)
{
	if (fPosition == where)
		return;
	fPosition = where;
	dirtySettings = true;
}


void
Settings::SetSplitWeight(float left, float right)
{
	if ((fLeftWeight == left) && (fRightWeight == right))
		return;
	fLeftWeight = left;
	fRightWeight = right;
	dirtySettings = true;
}


void
Settings::SetSplitCollapse(bool left, bool right)
{
	if ((fLeftCollapse == left) && (fRightCollapse == right))
		return;
	fLeftCollapse = left;
	fRightCollapse = right;
	dirtySettings = true;
}
