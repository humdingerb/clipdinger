/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "KeyFilter.h"


KeyFilter::KeyFilter()
	:
	BMessageFilter(B_PROGRAMMED_DELIVERY, B_ANY_SOURCE, B_KEY_DOWN)
{
}


KeyFilter::~KeyFilter()
{
}


filter_result
KeyFilter::Filter(BMessage* message, BHandler** target)
{
	int32 rawchar, mod;
	message->FindInt32("raw_char", &rawchar);
	message->FindInt32("modifiers", &mod);

	switch (rawchar) {
		case B_ESCAPE:
		{
			BLooper *loop = (*target)->Looper();
			if (loop) {
				BMessenger msgr(loop);
				msgr.SendMessage(msgESCAPE);
				return B_SKIP_MESSAGE;
			}
		}
		case B_DELETE:
		{
			BLooper *loop = (*target)->Looper();
			if (loop) {
				BMessenger msgr(loop);
				msgr.SendMessage(msgDELETE);
				return B_SKIP_MESSAGE;
			}
		}
		default:
		{
			return B_DISPATCH_MESSAGE;
		}
	}
}	
