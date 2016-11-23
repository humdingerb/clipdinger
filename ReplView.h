/*
 * Copyright 2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef REPLVIEW_H
#define REPLVIEW_H

#include <MessageFilter.h>
#include <StringView.h>
#include <View.h>


class ReplView : public BView
{
public:
							ReplView();
							ReplView(BMessage* data);
							~ReplView();

	static 	BArchivable* 	Instantiate(BMessage* archive);
	virtual status_t 		Archive(BMessage* data, bool deep = true) const;

	virtual void			AttachedToWindow();
	virtual void			MouseDown(BPoint point);
	void					MessageReceived(BMessage* msg);

	void					TruncateClip(float width);

private:
	BString					_GetClipboard();
	void					_LaunchClipdinger(BMessage* msg);
	static filter_result	_MessageFilter(BMessage* msg,
								BHandler** target, BMessageFilter* filter);
	void					_SetColor(rgb_color color);

	BStringView*			fContentsView;
	BString					fCurrentClip;
	bool					fReplicated;
};


#endif // DROPZONETAB_H
