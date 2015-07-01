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

#ifndef _INPUT_DEVICE_H
#define _INPUT_DEVICE_H

#include <InputServerDevice.h>
#include <List.h>
#include <OS.h>
#include <SupportDefs.h>


// export this for the input_server
extern "C" _EXPORT BInputServerDevice* instantiate_input_device();
 

class ClipdingerInputDevice : public BInputServerDevice {
public:
							ClipdingerInputDevice();
	virtual					~ClipdingerInputDevice();

	virtual status_t		InitCheck();
	virtual status_t		SystemShuttingDown();

	virtual status_t		Start(const char* device, void* cookie);
	virtual	status_t		Stop(const char* device, void* cookie);

	virtual status_t		Control(const char* device,
									void* cookie,
									uint32 code, 
									BMessage *message);
private:
	static int32			listener(void* arg);
	static thread_id		fThread;
};

#endif _INPUT_DEVICE_H
