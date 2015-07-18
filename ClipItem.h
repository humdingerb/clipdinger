/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPITEM_H
#define CLIPITEM_H

#include <Bitmap.h>
#include <Font.h>
#include <InterfaceDefs.h>
#include <ListItem.h>
#include <String.h>


class ClipItem : public BListItem {
public:
					ClipItem(BString clip, BString path, int32 time);
					~ClipItem();

	BString			GetClip() { return fClip; };
	BString			GetOrigin() { return fOrigin; };
	bigtime_t		GetTimeAdded() { return fTimeAdded; };
	void			SetTimeAdded(int32 time) { fTimeAdded = time; };
	rgb_color		SetColor(rgb_color color) { fColor = color; };
	BString			GetTitle() { return fTitle; };
	void			SetTitle(BString title) { fTitle = title; };

	virtual void	DrawItem(BView* view, BRect rect, bool complete);
	virtual	void	Update(BView* view, const BFont* finfo);

private:
	BString			fClip;
	BString			fTitle;
	BString			fOrigin;
	int32			fTimeAdded;
	rgb_color		fColor;

	BBitmap*		fOriginIcon;
};

#endif // CLIPITEM_H
