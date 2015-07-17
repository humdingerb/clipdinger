/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef APP_H
#define APP_H

#include <Alert.h>
#include <Application.h>
#include <TextView.h>

#include "ClipdingerSettings.h"
#include "MainWindow.h"

#define my_app dynamic_cast<App*>(be_app)

class App : public BApplication {
public:
						App();
	virtual				~App();

	virtual void		ReadyToRun();
	void				AboutRequested();

	ClipdingerSettings* Settings() { return &fSettings; }

	MainWindow*			fMainWindow;

private:
	ClipdingerSettings	fSettings;
};

#endif	// APP_H
