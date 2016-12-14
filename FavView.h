/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef FAVVIEW_H
#define FAVVIEW_H

#include <ListView.h>
#include <MenuItem.h>
#include <MessageRunner.h>
#include <PopUpMenu.h>


class FavView : public BListView {
public:
					FavView(const char* name);
					~FavView();

	virtual void	AttachedToWindow();
	virtual void	Draw(BRect rect);
	virtual	void	FrameResized(float width, float height);
	virtual bool	InitiateDrag(BPoint point, int32 index,
						bool wasSelected);
	virtual	void	MessageReceived(BMessage* message);

	virtual	void	KeyDown(const char* bytes, int32 numBytes);
	void			MouseDown(BPoint position);

private:
	void			_RenumberAll();
	void			_ShowPopUpMenu(BPoint screen);

	bool			fShowingPopUpMenu;
};

#endif // FAVVIEW_H
