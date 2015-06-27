/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */
 
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "Settings.h"

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


#define msgCLEAR_HISTORY	'clhi'
#define msgINSERT_CLIP		'iclp'
#define msgINSERT_FAVORITE	'ifav'
#define msgSETTINGS			'sett'

#define OUTPUT_PORT_NAME	"Clipdinger output port"

const int32	kControlKeys = B_COMMAND_KEY | B_SHIFT_KEY;


class MainWindow : public BWindow {
public:
					MainWindow();
	virtual			~MainWindow();
	
	bool			QuitRequested();
	void			MessageReceived(BMessage* message);
	void			UpdatedSettings(int32 limit);

private:
	void			_BuildLayout();
	bool			IsItemUnique(BString clipboardString);
	void			AddClip(BString clipboardString);
	BString			GetClipboard();
	void			PutClipboard(BListView* list);
	void			CropHistory(int32 limit);
	
private:
	int32			fLimit;
	
	BListView*		fHistory;
	BListView*		fFavorites;
	
	BScrollView*	fHistoryScrollView;
	BScrollView*	fFavoriteScrollView;

	Settings		fSettings;
};

#endif // MAIN_WINDOW_H
