/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "ClipListView.h"
#include "MainWindow.h"

ClipListView::ClipListView(const char* name)
		  :
		  BListView(BRect(), "name", B_SINGLE_SELECTION_LIST, B_WILL_DRAW)
{
}


ClipListView::~ClipListView()
{
}


void
ClipListView::Draw(BRect rect)
{
		SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
		FillRect(rect);
		BListView::Draw(rect);
}


void
ClipListView::FrameResized(float w, float h)
{
	BListView::FrameResized(w, h);
	
	for (int32 i = 0; i < CountItems(); i++) {
		BListItem *item = ItemAt(i);
		item->Update(this, be_plain_font);
	}
	Invalidate();
}
