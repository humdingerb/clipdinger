/*
 * Copyright 2015-2016. All rights reserved.
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
					ClipItem(BString clip, BString title, BString path,
						bigtime_t added, bigtime_t since);
					~ClipItem();

	virtual void	DrawItem(BView* view, BRect rect, bool complete = false);
	virtual	void	Update(BView* view, const BFont* finfo);

	BString			GetClip() { return fClip; };
	BString			GetOrigin() { return fOrigin; };

	BString			GetTitle();
	void			SetTitle(BString title, bool update = false);

	bigtime_t		GetTimeAdded() { return fTimeAdded; };
	void			SetTimeAdded(bigtime_t added) { fTimeAdded = added; };
	bigtime_t		GetTimeSince() { return fTimeSince; };
	void			SetTimeSince(bigtime_t since) { fTimeSince = since; };
	void			SetColor(rgb_color color) { fColor = color; };

private:
	BString			fClip;			// The actual clip, never touch!
	BString			fTitle;			// The optional user title.
	BString			fDisplayTitle;	// What's actually displayed
	bool			fUpdateNeeded;

	BString			fOrigin;
	BBitmap*		fOriginIcon;
	int32			fIconSize;
	bigtime_t		fTimeAdded;
	bigtime_t		fTimeSince;

	rgb_color		fColor;
};

#endif // CLIPITEM_H
