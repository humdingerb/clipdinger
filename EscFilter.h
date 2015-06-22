/*
 * Copyright 2010. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef ESC_FILTER_H
#define ESC_FILTER_H

#include <AppDefs.h>
#include <Handler.h>
#include <InterfaceDefs.h>
#include <Looper.h>
#include <MessageFilter.h>
#include <Messenger.h>

#include <stdio.h>


#define ESCAPE			'esca'


class EscFilter : public BMessageFilter {
public:
							EscFilter();
	virtual					~EscFilter();
	virtual filter_result 	Filter(BMessage *message, BHandler **target);
};

#endif // ESC_FILTER_H
