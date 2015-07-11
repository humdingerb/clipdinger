/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */
 
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Application.h>
#include <Clipboard.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <Size.h>
#include <SplitView.h>
#include <String.h>
#include <StringView.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "ClipListView.h"
#include "Settings.h"

const int32	kControlKeys = B_COMMAND_KEY | B_SHIFT_KEY;


class MainWindow : public BWindow {
public:
					MainWindow();
	virtual			~MainWindow();

	bool			QuitRequested();
	void			MessageReceived(BMessage* message);
	void			UpdatedSettings(int32 limit);

	Settings		fSettings;

private:
	void			_BuildLayout();
	void			_LoadHistory();
	void			_SaveHistory();

	void			MakeItemUnique(BString clip);
	void			AddClip(BString clip, BString path, int32 time);
	BString			GetClipboard();
	void			PutClipboard(BListView* list);
	void			CropHistory(int32 limit);
	
	int32			fLimit;
	int32			fLaunchTime;
	
	ClipListView*	fHistory;
//	BListView*		fFavorites;
	
	BScrollView*	fHistoryScrollView;
//	BScrollView*	fFavoriteScrollView;
};

#endif // MAIN_WINDOW_H
