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


#define msgENTRIES_25		'e025'
#define msgENTRIES_50		'e050'
#define msgENTRIES_100		'e100'
#define msgENTRIES_200		'e200'

#define msgCLEAR_HISTORY		'clhi'
#define msgINSERT_CLIP		'iclp'
#define msgINSERT_FAVORITE	'ifav'

#define OUTPUT_PORT_NAME	"Clipdinger output port"

const int32	kControlKeys = B_COMMAND_KEY | B_SHIFT_KEY;


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
	BMenuItem*		fLimit25;
	BMenuItem*		fLimit50;
	BMenuItem*		fLimit100;
	BMenuItem*		fLimit200;
	
	BListView*		fClipList;
	BListView*		fFavoriteList;
	
	BScrollView*	fClipScrollView;
	BScrollView*	fFavoriteScrollView;

};

#endif // MAIN_WINDOW_H
