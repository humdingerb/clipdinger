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
	BString			GetTitle() { return fTitle; };
	void			SetTitle(BString title) { fTitle = title; };
	void			SetDisplayTitle(BString display, bool update = false);
	void			SetFavNumber(int32 number) { fFavNumber = number; };

private:
	BString			fClip;
	BString			fDisplayTitle;
	BString			fTitle;
	int32			fFavNumber;
	bool			fUpdateNeeded;
};

#endif // FAVITEM_H
