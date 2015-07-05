/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef KEY_FILTER_H
#define KEY_FILTER_H

#include <AppDefs.h>
#include <Handler.h>
#include <InterfaceDefs.h>
#include <Looper.h>
#include <MessageFilter.h>
#include <Messenger.h>

#include "Constants.h"


class KeyFilter : public BMessageFilter {
public:
							KeyFilter();
	virtual					~KeyFilter();
	virtual filter_result 	Filter(BMessage* message, BHandler** target);
};

#endif // KEY_FILTER_H
