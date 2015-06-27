/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CLIPLISTVIEW_H
#define CLIPLISTVIEW_H

#include <ListView.h>


class ClipListView : public BListView {
public:
					ClipListView(const char* name);
					~ClipListView();
	virtual void	Draw(BRect rect);
	virtual	void	FrameResized(float w, float h);
};

#endif // CLIPLISTVIEW_H
