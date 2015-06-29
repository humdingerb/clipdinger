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

#include "InputDevice.h"

#include <InterfaceDefs.h>
#include <stdlib.h>
#include <syslog.h>


thread_id	ClipdingerInputDevice::fThread = B_ERROR;


BInputServerDevice* instantiate_input_device()
{
	return (new ClipdingerInputDevice());
}


ClipdingerInputDevice::ClipdingerInputDevice()
	:
	BInputServerDevice()
{
	input_device_ref clipdingerDevice = { 
		"Clipdinger input device", B_KEYBOARD_DEVICE, NULL};
	input_device_ref *clipdingerDeviceList[2] = {
		&clipdingerDevice, NULL};

	RegisterDevices(clipdingerDeviceList);
	
	char *ident = "Clipdinger device";
	int logopt = LOG_PID | LOG_CONS;
	int facility = LOG_USER;
	openlog(ident, logopt, facility);
	syslog(LOG_INFO, "Init Clipdinger device");
}


ClipdingerInputDevice::~ClipdingerInputDevice()
{
	// cleanup
}


status_t ClipdingerInputDevice::InitCheck()
{
	// do any init code that could fail here
	// you will be unloaded if you return false

	return (BInputServerDevice::InitCheck());
}


status_t ClipdingerInputDevice::SystemShuttingDown()
{
	// do any cleanup (ie. saving a settings file) when the
	// system is about to shut down

	return (BInputServerDevice::SystemShuttingDown());
}


status_t ClipdingerInputDevice::Start(const char* device, void* cookie)
{
	// start generating events
	// this is a hook function, it is called for you
	// (you should not call it yourself)

	fThread = spawn_thread(listener, device, B_LOW_PRIORITY, this);

	resume_thread(fThread);
	return B_NO_ERROR;
}


status_t ClipdingerInputDevice::Stop(const char* device, void* cookie)
{
	// stop generating events
	// this is a hook function, it is called for you
	// (you should not call it yourself)

	status_t err = B_OK;

	wait_for_thread(fThread, &err);
	fThread = B_ERROR;
	return B_NO_ERROR;
}


status_t ClipdingerInputDevice::Control(const char* device, void* cookie,
	uint32 code, BMessage* message)
{
	return B_NO_ERROR;
}


int32 ClipdingerInputDevice::listener(void* arg)
{
	port_id port = create_port(20, "Clipdinger output port");

	ClipdingerInputDevice* clipdingerDevice = (ClipdingerInputDevice *)arg;
	
	int32 code;
	while (read_port(port, &code, NULL, 0) == B_OK) {

		BMessage* event = new BMessage(B_KEY_DOWN);
		event->AddInt64("when", system_time());
		event->AddInt32("raw_char", 118);
		event->AddInt32("modifiers", B_COMMAND_KEY);
		event->AddInt8("byte", 'v');
		event->AddInt8("byte", 0);
		event->AddInt8("byte", 0);
		event->AddInt32("raw_char", 'v');
		event->AddString("bytes", "v");
		
		clipdingerDevice->EnqueueMessage(event);

		event = new BMessage(B_KEY_UP);
		event->AddInt64("when", system_time());
		event->AddInt32("raw_char", 118);
		event->AddInt32("modifiers", B_COMMAND_KEY);
		event->AddInt8("byte", 'v');
		event->AddInt8("byte", 0);
		event->AddInt8("byte", 0);
		event->AddInt32("raw_char", 'v');
		event->AddString("bytes", "v");
		
		clipdingerDevice->EnqueueMessage(event);

	syslog(LOG_INFO, "Clipdinger device: Added event");
	
		snooze(100000);
	}

	delete_port(port);
	return B_OK;
}
