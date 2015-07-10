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
#include <PopUpMenu.h>


class ClipListView : public BListView {
public:
					ClipListView(const char* name);
					~ClipListView();

//	virtual void	AttachedToWindow();
	virtual void	Draw(BRect rect);
	virtual	void	FrameResized(float w, float h);
	virtual	void	MessageReceived(BMessage* message);
	virtual void	Pulse();

	void			MouseDown(BPoint position);
	void			ShowPopUpMenu(BPoint screen);

private:
	bool			fShowingPopUpMenu;
};

#endif // CLIPLISTVIEW_H
