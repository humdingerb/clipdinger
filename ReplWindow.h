/*
 * Copyright 2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef REPL_WINDOW_H
#define REPL_WINDOW_H

#include <Window.h>

#include "ReplView.h"


class ReplWindow : public BWindow {
public:
					ReplWindow(BRect frame);
	virtual			~ReplWindow();

	virtual void	FrameResized(float width, float height);
	void			MessageReceived(BMessage* message);
	bool			QuitRequested();

private:
	void			_BuildLayout();
	ReplView*		fClipMonitorView;
};

#endif // REPL_WINDOW_H
