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
					ClipListItem(BString clip, BString path);
					~ClipListItem();

	BString			GetClip() { return fClip; };
	BString			GetOrigin() { return fOrigin; };
	virtual void	DrawItem(BView*, BRect, bool);
	virtual	void	Update(BView*, const BFont*);

private:
	BString			fClip;
	BString			fTitle;
	BString			fOrigin;

	BBitmap*		fOriginIcon;
};

#endif // CLIPLISTITEM_H
