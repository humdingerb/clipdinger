/*
 * Copyright 2014. All rights reserved.
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
#include <MenuItem.h>
#include <String.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const int kBitmapSize = 10;


class ClipListItem : public BListItem {
public:
					ClipListItem(BString clip);
					~ClipListItem();
	virtual void	DrawItem(BView*, BRect, bool);
	virtual	void	Update(BView*, const BFont*);

private:
	BString			fClip;
	float			fFontHeight;
};

#endif // CLIPLISTITEM_H
