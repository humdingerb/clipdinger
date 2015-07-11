/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Font.h>
#include <Message.h>

#include "Constants.h"
#include "Settings.h"


Settings::Settings()
{
	BPath path;
	BMessage msg;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(kSettingsFolder);
		if (ret == B_OK) {
			path.Append(kSettingsFile);
			BFile file(path.Path(), B_READ_ONLY);

			if (file.InitCheck() != B_OK || (msg.Unflatten(&file) != B_OK)) {
				fLimit = kDefaultLimit;
				fFadeSpeed = kDefaultFadeSpeed;
				fFadeStep = kDefaultFadeStep;
				fPosition.Set(-1, -1, -1, -1);
			} else {
				if (msg.FindInt32("limit", &fLimit) != B_OK)
					fLimit = kDefaultLimit;

				if (msg.FindInt32("fadespeed", &fFadeSpeed) != B_OK)
					fFadeSpeed = kDefaultFadeSpeed;

				if (msg.FindFloat("fadestep", &fFadeStep) != B_OK)
					fFadeStep = kDefaultFadeStep;

				if (msg.FindRect("windowlocation", &fPosition) != B_OK)
					fPosition.Set(-1, -1, -1, -1);

				originalLimit = fLimit;
				originalFadeSpeed = fFadeSpeed;
				originalFadeStep = fFadeStep;
				originalPosition = fPosition;
			}
		}
	}
}


Settings::~Settings()
{
	if (originalPosition == fPosition &&
		originalLimit == fLimit &&
		originalFadeSpeed == fFadeSpeed &&
		originalFadeStep == fFadeStep)
			return;

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
			msg.AddInt32("fadespeed", fFadeSpeed);
			msg.AddFloat("fadestep", fFadeStep);
			msg.AddRect("windowlocation", fPosition);
			msg.Flatten(&file);
		}
	}
}


int32
Settings::Limit()
{
	return fLimit;
}


void
Settings::SetLimit(int32 limit)
{
	fLimit = limit;
}


int32
Settings::FadeSpeed()
{
	return fFadeSpeed;
}


void
Settings::SetFadeSpeed(int32 speed)
{
	fFadeSpeed = speed;
}


float
Settings::FadeStep()
{
	return fFadeStep;
}


void
Settings::SetFadeStep(float step)
{
	fFadeStep = step;
}


BRect
Settings::WindowPosition()
{
	return fPosition;
}


void
Settings::SetWindowPosition(BRect where)
{
	fPosition = where;
}
