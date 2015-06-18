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
#include <Button.h>
#include <Clipboard.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <ListView.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <Size.h>
#include <SplitView.h>
#include <String.h>
#include <StringItem.h>
#include <StringView.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


#define ENTRIES_25		'e025'
#define ENTRIES_50		'e050'
#define ENTRIES_100		'e100'
#define ENTRIES_200		'e200'

#define CLEAR_HISTORY		'clhi'
#define MSG_INSERT_CLIP		'iclp'
#define MSG_INSERT_FAVORITE	'ifav'

class MainWindow : public BWindow {
public:
					MainWindow(BRect rect);
	virtual			~MainWindow();
	
	bool			QuitRequested();
	void			MessageReceived(BMessage* message);

private:
	void			_BuildLayout();
	bool			IsItemUnique(BString clipboardString);
	void			AddClip(BString clipboardString);
	BString			GetClipboard();
	void			PutClipboard(BListView* list);
	void			CropHistory(int limit);
	
private:
	int32			fLimit;
	BMenuItem		*fLimit25;
	BMenuItem		*fLimit50;
	BMenuItem		*fLimit100;
	BMenuItem		*fLimit200;

//	BSplitView		*fMainSplitView;
//	BSplitView		*fRightSplitView;
	
	BListView		*fClipList;
	BListView		*fFavoriteList;
//	BListView		*fSetList;
	
	BScrollView		*fClipScrollView;
	BScrollView		*fFavoriteScrollView;
//	BScrollView		*fSetScrollView;

//	BButton			*fPasteButton;	
//	BButton			*fAddButton;
//	BButton			*fRemoveButton;
//	BButton			*fEditButton;
};

#endif // MAIN_WINDOW_H
