/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef FAVITEM_H
#define FAVITEM_H

#include <Font.h>
#include <InterfaceDefs.h>
#include <ListItem.h>
#include <String.h>


class FavItem : public BListItem {
public:
					FavItem(BString clip, BString title, int32 favnumber);
					~FavItem();

	virtual void	DrawItem(BView* view, BRect rect, bool complete = false);
	virtual	void	Update(BView* view, const BFont* finfo);

	BString			GetClip() { return fClip; };
	BString			GetTitle();
	void			SetTitle(BString title, bool update = false);
	void			SetDisplayTitle(BString string, bool update = false);
	void			SetFavNumber(int32 number) { fFavNumber = number; };

private:
	BString			fClip;			// The actual clip, never touch!
	BString			fTitle;			// The optional user title.
	BString			fDisplayTitle;	// What's actually displayed
	bool			fUpdateNeeded;
	int32			fFavNumber;
};

#endif // FAVITEM_H
