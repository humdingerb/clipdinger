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
					ClipListItem(BString clip, entry_ref ref);
					~ClipListItem();

	BString			GetClip() { return fClip->String(); };
	entry_ref		GetOrigin() { return fOriginRef; };
	virtual void	DrawItem(BView*, BRect, bool);
	virtual	void	Update(BView*, const BFont*);

private:
	BString*		fClip;
	BString*		fTitle;
	entry_ref		fOriginRef;

	BBitmap*		fOriginIcon;
	font_height		fFontHeight;
};

#endif // CLIPLISTITEM_H
