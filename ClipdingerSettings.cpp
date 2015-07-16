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
#include "ClipdingerSettings.h"


ClipdingerSettings::ClipdingerSettings()
	:
	fLimit(kDefaultLimit),
	fFade(kDefaultFade),
	fFadeDelay(kDefaultFadeDelay),
	fFadeStep(kDefaultFadeStep)
{
	BPath path;
	BMessage msg;
	fPosition.Set(-1, -1, -1, -1);

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(kSettingsFolder);
		if (ret == B_OK) {
			path.Append(kSettingsFile);
			BFile file(path.Path(), B_READ_ONLY);
			
			if ((file.InitCheck() == B_OK) && (msg.Unflatten(&file) == B_OK)) {
				if (msg.FindInt32("limit", &fLimit) != B_OK)
					fLimit = kDefaultLimit;

				if (msg.FindInt32("fade", &fFade) != B_OK)
					fFade = kDefaultFade;

				if (msg.FindInt32("fadedelay", &fFadeDelay) != B_OK)
					fFadeDelay = kDefaultFadeDelay;

				if (msg.FindInt32("fadestep", &fFadeStep) != B_OK)
					fFadeStep = kDefaultFadeStep;

				if (msg.FindRect("windowlocation", &fPosition) != B_OK)
					fPosition.Set(-1, -1, -1, -1);

				originalLimit = fLimit;
				originalFade = fFade;
				originalFadeDelay = fFadeDelay;
				originalFadeStep = fFadeStep;
				originalPosition = fPosition;
			}
		}
	}
}


ClipdingerSettings::~ClipdingerSettings()
{
	if (originalLimit == fLimit &&
		originalPosition == fPosition &&
		originalFade == fFade &&
		originalFadeDelay == fFadeDelay &&
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
			msg.AddInt32("fade", fFade);
			msg.AddInt32("fadedelay", fFadeDelay);
			msg.AddInt32("fadestep", fFadeStep);
			msg.AddRect("windowlocation", fPosition);
			msg.Flatten(&file);
		}
	}
}


bool
ClipdingerSettings::Lock()
{
	return fLock.Lock();
}


void
ClipdingerSettings::Unlock()
{
	fLock.Unlock();
}
