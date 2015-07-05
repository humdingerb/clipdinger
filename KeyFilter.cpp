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
	int32 key, rawchar, mod;
	message->FindInt32("key", &key);
	message->FindInt32("raw_char", &rawchar);
	message->FindInt32("modifiers", &mod);

	switch (key) {
		case 0x28:		// key 'W'
		{
			if (mod & B_COMMAND_KEY) {
				BLooper *loop = (*target)->Looper();
				if (loop) {
					BMessenger msgr(loop);
					msgr.SendMessage(ESCAPE);
					return B_SKIP_MESSAGE;
				}
			}
			return B_DISPATCH_MESSAGE;
		}
	}
	switch (rawchar) {
		case B_ESCAPE:
		{
			BLooper *loop = (*target)->Looper();
			if (loop) {
				BMessenger msgr(loop);
				msgr.SendMessage(ESCAPE);
				return B_SKIP_MESSAGE;
			}
			break;
		}
		case B_DELETE:
		{
			BLooper *loop = (*target)->Looper();
			if (loop) {
				BMessenger msgr(loop);
				msgr.SendMessage(DELETE);
				return B_SKIP_MESSAGE;
			}
			break;
		}
		default:
		{
			return B_DISPATCH_MESSAGE;
			break;
		}
	}
}	
