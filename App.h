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

#include "MainWindow.h"


class App : public BApplication {
public:
					App();
	virtual			~App();

	void			AboutRequested();
	void			MessageReceived(BMessage* message);
//	virtual void	Pulse();

	MainWindow*		fMainWindow;
	port_id			fPort;
	BMessageRunner*	fPortRunner;
};

#endif	// APP_H
