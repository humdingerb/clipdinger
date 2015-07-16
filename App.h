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
#include <MessageRunner.h>
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
	void				MessageReceived(BMessage* message);

	ClipdingerSettings* Settings() { return &fSettings; }

	MainWindow*			fMainWindow;

private:
	port_id				fPort;
	BMessageRunner*		fPortRunner;
	ClipdingerSettings	fSettings;
};

#endif	// APP_H
