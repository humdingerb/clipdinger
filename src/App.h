/*
 * Copyright 2015-2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef APP_H
#define APP_H

#include <Application.h>
#include <TextView.h>

#include "Settings.h"
#include "MainWindow.h"
#include "ReplWindow.h"
#include "SettingsWindow.h"


#define my_app dynamic_cast<App*>(be_app)

class App : public BApplication {
public:
						App();
	virtual				~App();

	virtual void		ReadyToRun();
	virtual bool		QuitRequested();
	void				AboutRequested();
	void				MessageReceived(BMessage* msg);

	Settings*			GetSettings() { return &fSettings; }

	MainWindow*			fMainWindow;

private:
	Settings			fSettings;
	ReplWindow*			fReplWindow;
	SettingsWindow*		fSettingsWindow;
};

#endif	// APP_H
