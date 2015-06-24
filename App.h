/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef APP_H
#define APP_H

#include "MainWindow.h"

#include <Alert.h>
#include <Application.h>
#include <TextView.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define INPUT_PORT_NAME		"Clipdinger input port"


class App : public BApplication {
public:
					App();
	virtual			~App();

	void			AboutRequested();
	virtual void	Pulse();
	
	MainWindow*		fMainWindow;
	port_id			fPort;
};

#endif	// APP_H
