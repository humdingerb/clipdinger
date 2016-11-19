/*
 * Copyright 2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef REPLVIEW_H
#define REPLVIEW_H

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
	void					MessageReceived(BMessage* msg);

	void					TruncateClip(float width);

private:
	BString					_GetClipboard();

	bool					fReplicated;
	BStringView*			fContentsView;
	BString					fCurrentClip;
};


#endif // DROPZONETAB_H
