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

	BString			GetClip() { return fClip; };
	BString			GetOrigin() { return fOrigin; };
	bigtime_t		GetTimeAdded() { return fTimeAdded; };
	void			SetTimeAdded(int32 time) { fTimeAdded = time; };

	virtual void	DrawItem(BView* view, BRect rect, bool complete);
	virtual	void	Update(BView* view, const BFont* finfo);

private:
	BString			fClip;
	BString			fTitle;
	BString			fOrigin;
	int32			fTimeAdded;

	BBitmap*		fOriginIcon;
};

#endif // CLIPLISTITEM_H
