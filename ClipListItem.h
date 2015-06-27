/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPLISTITEM_H
#define CLIPLISTITEM_H


#include <Font.h>
#include <InterfaceDefs.h>
#include <ListItem.h>
#include <String.h>


const int32 MAX_TITLE_CHARS = 200;


class ClipListItem : public BListItem {
public:
					ClipListItem(BString clip);
					~ClipListItem();

	BString			GetClip() { return fClip->String(); };
	virtual void	DrawItem(BView*, BRect, bool);
	virtual	void	Update(BView*, const BFont*);

private:
	BString*		fClip;
	BString*		fTitle;
	font_height		fFontHeight;
};

#endif // CLIPLISTITEM_H
