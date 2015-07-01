/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 *
 * Minimal changes by Humdinger, humdingerb@gmail.com 
 */

#include "InputFilter.h"
#include <InterfaceDefs.h>
#include <Looper.h>
#include <Message.h>
#include <Messenger.h>
#include <OS.h>

#include <stdio.h>
// #include <syslog.h>

//------------------------------------------------------------------------

extern "C" BInputServerFilter* instantiate_input_filter()
{
	return new InputFilter();
}

//------------------------------------------------------------------------

InputFilter::InputFilter()
{
//	char *ident = "Clipdinger filter";
//	int logopt = LOG_PID | LOG_CONS;
//	int facility = LOG_USER;
//	openlog(ident, logopt, facility);
//	syslog(LOG_INFO, "Init Clipdinger filter");
}


filter_result InputFilter::Filter(BMessage* message, BList* outList)
{
	
	const int32	kControlKeys = B_COMMAND_KEY | B_SHIFT_KEY;
	
	if (message->what == B_KEY_DOWN) {
		int32 raw;
		int32 mods;
		
		if ((message->FindInt32("raw_char", 0, (int32 *)&raw) == B_OK)
			&& (message->FindInt32("modifiers", (int32 *)&mods) == B_OK)
			&& (raw==118) // v
			&& ((mods & kControlKeys) == kControlKeys)) {
			
			port_id port = find_port("Clipdinger input port");

			if (port!=B_NAME_NOT_FOUND) {
				write_port(port, 'CtSV', NULL, 0);
//				syslog(LOG_INFO, "Clipdinger filter: Writing CtSV to write port");
				return B_SKIP_MESSAGE;
			}
		}
	}
	return B_DISPATCH_MESSAGE;
}


status_t InputFilter::InitCheck()
{
	return B_OK;
}
