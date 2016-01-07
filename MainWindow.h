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
#include <CheckBox.h>
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

#include "ClipView.h"
#include "EditWindow.h"
#include "FavView.h"
#include "SettingsWindow.h"

const int32	kControlKeys = B_COMMAND_KEY | B_SHIFT_KEY;


class MainWindow : public BWindow {
public:
					MainWindow(BRect frame);
	virtual			~MainWindow();

	bool			QuitRequested();
	void			MessageReceived(BMessage* message);

private:
	void			_BuildLayout();
	void			_LoadHistory();
	void			_SaveHistory();
	void			_LoadFavorites();
	void			_SaveFavorites();
	void			_SetSplitview();

	void			MakeItemUnique(BString clip);
	void			AddClip(BString clip, BString path, int32 time);
	void			AddFav();
	BString			GetClipboard();
	void			PutClipboard(BString text);
	void			CropHistory(int32 limit);
	void			AutoPaste();
	void			MoveClipToTop();
	void			UpdateColors();
	void			RenumberFavorites(int32 start);

	int32			fLimit;
	int32			fAutoPaste;
	int32			fLaunchTime;

	BSplitView*		fMainSplitView;
	ClipView*		fHistory;
	FavView*		fFavorites;

	BScrollView*	fHistoryScrollView;
	BScrollView*	fFavoriteScrollView;

	BCheckBox*		fPauseCheckBox;
	BButton*		fButtonUp;
	BButton*		fButtonDown;

	EditWindow*		fEditWindow;
	SettingsWindow*	fSettingsWindow;
};

#endif // MAIN_WINDOW_H
