/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPVIEW_H
#define CLIPVIEW_H

#include <ListView.h>
#include <MenuItem.h>
#include <MessageRunner.h>


class ClipView : public BListView {
public:
					ClipView(const char* name);
					~ClipView();

	virtual void	AttachedToWindow();
	virtual void	Draw(BRect rect);
	virtual	void	FrameResized(float width, float height);
	virtual	void	MessageReceived(BMessage* message);
	virtual	void	KeyDown(const char* bytes, int32 numBytes);
	void			MouseDown(BPoint position);

	void			AdjustColors();
	void			ShowPopUpMenu(BPoint screen);

private:
	bool			fShowingPopUpMenu;
	BMessageRunner*	fRunner;
};

#endif // CLIPVIEW_H
