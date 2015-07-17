/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPLISTVIEW_H
#define CLIPLISTVIEW_H

#include <ListView.h>
#include <MenuItem.h>
#include <MessageRunner.h>
#include <PopUpMenu.h>


class ClipListView : public BListView {
public:
					ClipListView(const char* name);
					~ClipListView();

	virtual void	AttachedToWindow();
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

#endif // CLIPLISTVIEW_H
