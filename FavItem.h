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

	BString			GetClip() { return fClip; };
	BString			GetTitle() { return fTitle; };
	void			SetTitle(BString title) { fTitle = title; };
	void			SetFavNumber(int32 number) { fFavNumber = number; };

	virtual void	DrawItem(BView* view, BRect rect, bool complete);
	virtual	void	Update(BView* view, const BFont* finfo);

private:
	BString			fClip;
	BString			fTitle;
	int32			fFavNumber;
};

#endif // FAVITEM_H
