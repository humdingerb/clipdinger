/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPLISTITEM_H
#define CLIPLISTITEM_H

#include <Bitmap.h>
#include <Font.h>
#include <InterfaceDefs.h>
#include <ListItem.h>
#include <String.h>


class ClipListItem : public BListItem {
public:
					ClipListItem(BString clip, BString path, int32 time);
					~ClipListItem();

	BString			GetClip();
	BString			GetOrigin();
	bigtime_t		GetTimeAdded();
	void			SetTimeAdded(int32 time);

	virtual void	DrawItem(BView*, BRect, bool);
	virtual	void	Update(BView*, const BFont*);

private:
	BString			fClip;
	BString			fTitle;
	BString			fOrigin;
	int32			fTimeAdded;

	BBitmap*		fOriginIcon;
};

#endif // CLIPLISTITEM_H
