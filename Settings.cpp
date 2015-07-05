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
				fPosition.Set(-1, -1, -1, -1);
			} else {
				msg.FindInt32("limit", &fLimit);
				msg.FindRect("windowlocation", &fPosition);
				originalLimit = fLimit;
				originalPosition = fPosition;
			}
		}
	}
}


Settings::~Settings()
{
	if (originalPosition == fPosition &&
		originalLimit == fLimit)
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
			msg.AddRect("windowlocation", fPosition);
			msg.Flatten(&file);
		}
	}
}


void
Settings::SetLimit(int32 limit)
{
	fLimit = limit;
}


void
Settings::SetWindowPosition(BRect where)
{
	fPosition = where;
}
