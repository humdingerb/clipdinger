/*
 * Copyright 2015-2016. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Application.h>
#include <Button.h>
#include <CardLayout.h>
#include <CheckBox.h>
#include <Clipboard.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ObjectList.h>
#include <ScrollView.h>
#include <Size.h>
#include <SplitView.h>
#include <String.h>
#include <StringView.h>
#include <TextControl.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "ClipItem.h"
#include "ClipView.h"
#include "EditWindow.h"
#include "FavView.h"

const int32	kControlKeys = B_COMMAND_KEY | B_SHIFT_KEY;


class MainWindow : public BWindow {
public:
					MainWindow(BRect frame);
	virtual			~MainWindow();

	bool			QuitRequested();
	void			MessageReceived(BMessage* message);
	void			WindowActivated(bool active);

	bool			GetHistoryActiveFlag();
	void			SetHistoryActiveFlag(bool flag);
	BString			GetFilterText() { return fFilterControl->Text(); }

	BObjectList<ClipItem>		fBackup;
	ClipView*		fHistory;
	FavView*		fFavorites;

private:
	void			_BuildLayout();
	void			_SetSplitview();
	void			_ResetFilter();
	void			_BackupHistory();
	void			_RestoreHistory();

	void			_LoadHistory();
	void			_SaveHistory();
	void			_LoadFavorites();
	void			_SaveFavorites();
	void			_OpenHelp();

	void			_AddClip(BString clip, BString title, BString path,
						bigtime_t time, bigtime_t since);
	void			_MakeItemUnique(BString clip);
	void			_MoveClipToTop();
	void			_CropHistory(int32 limit);
	bool			_CheckNetworkConnection();
	static status_t	_UploadClip(void* self);

	BString			_GetClipboard();
	void			_PutClipboard(BString text);

	void			_AutoPaste();
	void			_UpdateControls();
	void			_UpdateColors();

	int32			fLimit;
	int32			fAutoPaste;
	bigtime_t		fLaunchTime;
	thread_id		fThread;

	BSplitView*		fMainSplitView;
	bool			fHistoryActiveFlag;	// true if fHistory view is the active one
	bool			fDoQuit; // if true, QuitRequested() quits, otherwise minimize

	BTextControl*	fFilterControl;

	BButton*		fButtonUp;
	BButton*		fButtonDown;
	BMenuItem*		fMenuAdd;
	BMenuItem*		fMenuEdit;
	BMenuItem*		fMenuDelete;
	BMenuItem*		fMenuPaste;
	BMenuItem*		fMenuClearFav;
	BMenuItem*		fMenuPauseFading;

	EditWindow*		fEditWindow;
};

#endif // MAIN_WINDOW_H
