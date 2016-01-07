/*
 * Copyright 2015 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef KEYCATCHER_H
#define KEYCATCHER_H_H

#include <View.h>


class KeyCatcher : public BView {
public:
					KeyCatcher(const char* name);
					~KeyCatcher();

	virtual void	AttachedToWindow();
	virtual	void	KeyDown(const char* bytes, int32 numBytes);
};

#endif // KEYCATCHER_H_H
