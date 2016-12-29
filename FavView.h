/*
 * Copyright 2015-2016. All rights reserved.
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
	virtual bool	InitiateDrag(BPoint point, int32 dragIndex,
						bool wasSelected);
	virtual	void	MakeFocus(bool focused = true);
	virtual	void	MessageReceived(BMessage* message);

	virtual	void	KeyDown(const char* bytes, int32 numBytes);
	void			MouseDown(BPoint position);
	void			MouseUp(BPoint position);
	virtual	void 	MouseMoved(BPoint where, uint32 transit,
						const BMessage* dragMessage);

	void			RenumberFKeys();

private:
	void			_ShowPopUpMenu(BPoint screen);

	bool			fShowingPopUpMenu;
	BRect			fDropRect;
};

#endif // FAVVIEW_H
