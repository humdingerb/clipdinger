/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <strings.h>

#include "KeyCatcher.h"
#include "Constants.h"
#include "MainWindow.h"


KeyCatcher::KeyCatcher(const char* name)
	:
	BView(name, 0)
{
}


KeyCatcher::~KeyCatcher()
{
}


void
KeyCatcher::AttachedToWindow()
{
	SetEventMask(B_KEYBOARD_EVENTS);
	BView::AttachedToWindow();
}


void
KeyCatcher::KeyDown(const char* bytes, int32 numBytes)
{
	static const int32 kModifiers = B_SHIFT_KEY | B_COMMAND_KEY;

	if (strcasecmp(bytes, "v") == 0
			&& (modifiers() & kModifiers) == kModifiers) {
		Window()->Minimize(false);
		Window()->Activate(true);
	}
	else if (Window()->IsActive()) {
		switch (bytes[0]) {
			case B_ESCAPE:
			{
				Looper()->PostMessage(ESCAPE);
				break;
			}
			case B_FUNCTION_KEY:
			{
				int32 fkey = 0;
				if (Window() != NULL && Window()->CurrentMessage() != NULL)
					fkey = Window()->CurrentMessage()->FindInt32("key");

				BMessenger messenger(Looper());
				BMessage message(INSERT_FAVORITE);
				message.AddInt32("index", fkey - 2); // F1 == 0x02
				messenger.SendMessage(&message);
				break;
			}
		}
	}
}
