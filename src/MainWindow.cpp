/*
 * Copyright 2015-2022. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Alert.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <NetworkInterface.h>
#include <NetworkRoster.h>
#include <Path.h>
#include <PathFinder.h>
#include <Roster.h>
#include <Screen.h>

#include <algorithm>

#include "App.h"
#include "ClipItem.h"
#include "Constants.h"
#include "FavItem.h"
#include "IconMenuItem.h"
#include "KeyCatcher.h"
#include "MainWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"

//#define HISTORY_VIEW		0
//#define FILTER_VIEW 		1

MainWindow::MainWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE_SYSTEM_NAME("Clipdinger"), B_TITLED_WINDOW,
		B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS, B_ALL_WORKSPACES),
	fDoQuit(false)
{
	KeyCatcher* catcher = new KeyCatcher("catcher");
	AddChild(catcher);
	catcher->Hide();

	_BuildLayout();
	_SetSplitview();

	if (frame == BRect(-1, -1, -1, -1)) {
		CenterOnScreen();
		ResizeTo(500, 400);
	} else {
		// make sure window is on screen
		BScreen screen(this);
		if (!screen.Frame().InsetByCopy(10, 10).Intersects(Frame()))
			CenterOnScreen();
	}
	Settings* settings = my_app->GetSettings();
	int32 fade = 0;
	if (settings->Lock()) {
		fAutoPaste = settings->GetAutoPaste();
		fLimit = settings->GetLimit();
		fade = settings->GetFade();
		settings->Unlock();
	}

	fMenuPauseFading->SetEnabled(fade);

	fLaunchTime = real_time_clock();

	_LoadHistory();
	_LoadFavorites();

	if (fFavorites->CountItems() > 0)
		_UpdateControls();

	if (!fHistory->IsEmpty())
		fHistory->Select(0);
	if (!fFavorites->IsEmpty())
		fFavorites->Select(0);

	if (_GetClipboard() == "") {
		if (!fHistory->IsEmpty()) {
			ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(0));
			BString text(item->GetClip());
			_PutClipboard(text);
		}
	}
	be_clipboard->StartWatching(this);
	PostMessage(B_CLIPBOARD_CHANGED);
}


MainWindow::~MainWindow()
{
}


bool
MainWindow::QuitRequested()
{
	// To avoid the window close button quitting the app. Minimize instead.
	if (fDoQuit == false) {
		PostMessage(MINIMIZE);
		return false;
	}

	BString filter = fFilterControl->Text();
	if (filter != "")
		_ResetFilter();

// we already save history and favorites with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
//	_SaveHistory();
//	_SaveFavorites();

	Settings* settings = my_app->GetSettings();
	if (settings->Lock()) {
		float leftWeight = fMainSplitView->ItemWeight((int32) 0);
		float rightWeight = fMainSplitView->ItemWeight(1);
		bool leftCollapse = fMainSplitView->IsItemCollapsed((int) 0);
		bool rightCollapse = fMainSplitView->IsItemCollapsed(1);
		settings->SetSplitWeight(leftWeight, rightWeight);
		settings->SetSplitCollapse(leftCollapse, rightCollapse);
		settings->SetWindowPosition(ConvertToScreen(Bounds()));
		settings->Unlock();
	}

	status_t status;
	wait_for_thread(fThread, &status);

	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_COLORS_UPDATED:
		{
			if (message->HasColor(ui_color_name(B_LIST_BACKGROUND_COLOR))) {
				fHistory->MakeEmpty();
				_LoadHistory();
			}
			break;
		}
		case B_CLIPBOARD_CHANGED:
		{
			BString clip(_GetClipboard());
			if (clip.Length() == 0)
				break;
			fHistory->DeselectAll();

			app_info info;
			BPath path;
			be_roster->GetActiveAppInfo(&info);
			BEntry entry(&info.ref);
			entry.GetPath(&path);

			BString filter = fFilterControl->TextView()->Text();
			if (filter != "") {
				_RestoreHistory();
				fBackup.MakeEmpty();
			}

			_MakeItemUnique(clip);
			bigtime_t time(real_time_clock());
			_AddClip(clip, NULL, path.Path(), time, time);

			if (filter != "")
				PostMessage(FILTER_INPUT);

			fHistory->Select(0);
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveHistory();
			break;
		}
		case MINIMIZE:
		{
			BString filter = fFilterControl->Text();
			if (filter != "")
				_ResetFilter();
			Minimize(true);
			break;
		}
		case DOQUIT:
		{
			fDoQuit = true;
			QuitRequested();
			break;
		}
		case ESCAPE:
		{
			BString filter = fFilterControl->Text();
			if (filter != "")
				_ResetFilter();
			else
				Minimize(true);
			break;
		}
		case PAUSE:
		{
			int32 pause = fMenuPauseFading->IsMarked();
			fMenuPauseFading->SetMarked(!pause);
			SetTitle(pause ? B_TRANSLATE_SYSTEM_NAME("Clipdinger")
						   : B_TRANSLATE("Clipdinger (fading paused)"));

			Settings* settings = my_app->GetSettings();
			if (settings->Lock()) {
				settings->SetFadePause(!pause);
				settings->Unlock();
			}
			break;
		}
		case DELETE:
		{
			if (GetHistoryActiveFlag() && !fHistory->IsEmpty()) {
				int32 index = fHistory->CurrentSelection();
				if (index < 0)
					break;
				fHistory->RemoveItem(index);

				int32 count = fHistory->CountItems();
				// Only item left deleted: clear clipboard
				if (count == 0) {
					_PutClipboard("");
					_SaveHistory();
					break;
				}

				fHistory->Select((index > count - 1) ? count - 1 : index);

				if (index == 0) {
					ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(0));
					BString text(item->GetClip());
					_PutClipboard(text);
				}
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveHistory();
			} else if (!GetHistoryActiveFlag() && !fFavorites->IsEmpty()) {
				int32 index = fFavorites->CurrentSelection();
				if (index < 0)
					break;
				fFavorites->RemoveItem(index);

				fFavorites->RenumberFKeys();
				int32 count = fFavorites->CountItems();
				fFavorites->Select((index > count - 1) ? count - 1 : index);
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveFavorites();
			}
			break;
		}
		case EDIT_TITLE:
		{
			BString text;
			if (GetHistoryActiveFlag() && !fHistory->IsEmpty()) {
				int32 index = fHistory->CurrentSelection();
				if (index < 0)
					break;

				ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(index));
				text = item->GetTitle();

			} else if (!GetHistoryActiveFlag() && !fFavorites->IsEmpty()) {
				int32 index = fFavorites->CurrentSelection();
				if (index < 0)
					break;

				FavItem* item = dynamic_cast<FavItem*>(fFavorites->ItemAt(index));
				if ((text = item->GetTitle()) == "")
					text = item->GetClip();
			} else
				break;

			fEditWindow = new EditWindow(Frame(), text);
			fEditWindow->Show();
			break;
		}
		case UPDATE_TITLE:
		{
			BString newTitle;
			if (GetHistoryActiveFlag() && !fHistory->IsEmpty()) {
				int32 index = fHistory->CurrentSelection();
				if (index < 0)
					break;

				if (message->FindString("edit_title", &newTitle) == B_OK) {
					ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(index));
					item->SetTitle(newTitle, true);
					fHistory->InvalidateItem(index);
				}
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveHistory();
			} else if (!GetHistoryActiveFlag() && !fFavorites->IsEmpty()) {
				int32 index = fFavorites->CurrentSelection();
				if (index < 0)
					break;

				if (message->FindString("edit_title", &newTitle) == B_OK) {
					FavItem* item = dynamic_cast<FavItem*>(fFavorites->ItemAt(index));
					item->SetTitle(newTitle, true);
					fFavorites->InvalidateItem(index);
				}
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveFavorites();
			}
			break;
		}
		case B_SIMPLE_DATA:
		case FAV_ADD:
		{
			if (message->WasDropped()) {
				BRect favScreen = fFavorites->ConvertToScreen(fFavorites->Bounds());
				BPoint dropPoint = message->DropPoint();
				if (!(favScreen.Contains(dropPoint)))
					break;
			}
			int32 command = -1;
			// only accept drops from inside Clipdinger
			if (message->FindInt32("clipdinger_command", &command) != B_OK)
				break;

			int32 index = fFavorites->CountItems();
			if (command == FAV_ADD) {
				ClipItem* clip
					= dynamic_cast<ClipItem*>(fHistory->ItemAt(fHistory->CurrentSelection()));
				if (clip == NULL)
					break;

				BString title(clip->GetTitle());
				BString contents(clip->GetClip());
				if (title == contents)
					title = "";

				FavItem* favItem = NULL;
				bool isDuplicate = false;
				for (int32 i = 0; i < index; ++i) {
					favItem = static_cast<FavItem*>(fFavorites->ItemAt(i));
					if (favItem->GetClip() == contents) {
						isDuplicate = true;
						break;
					}
				}

				if (isDuplicate) {
					BString text(B_TRANSLATE("This clip is already a favorite"));
					BString favTitle(favItem->GetTitle());
					if (favTitle != contents) {
						text += " ('";
						text += favTitle;
						text += "')";
					}
					text += '.';

					BAlert* alert = new BAlert("error", text, B_TRANSLATE("OK"));
					alert->Go();
					break;
				}

				fFavorites->AddItem(new FavItem(contents, title, index + 1));
			}
			// move Fav to where it was dropped
			if (message->WasDropped()) {
				BMessenger messenger(fFavorites);
				BMessage msg(FAV_DRAGGED);
				if (message->FindInt32("index", &index) != B_OK)
					index = fFavorites->CountItems() - 1;
				msg.AddInt32("index", index);
				msg.AddPoint("_drop_point_", message->DropPoint());
				messenger.SendMessage(&msg);
			} else
				fFavorites->RenumberFKeys();
			_UpdateControls();
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveFavorites();
			break;
		}
		case FAV_DOWN:
		{
			int32 index = fFavorites->CurrentSelection();
			int32 last = fFavorites->CountItems() - 1;
			if ((index == last) || (index < 0))
				break;

			fFavorites->SwapItems(index, index + 1);
			fFavorites->Select(index + 1);
			fFavorites->RenumberFKeys();
			_UpdateControls();
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveFavorites();
			break;
		}
		case FAV_UP:
		{
			int32 index = fFavorites->CurrentSelection();
			if (index < 1)
				break;

			fFavorites->SwapItems(index, index - 1);
			fFavorites->Select(index - 1);
			fFavorites->RenumberFKeys();
			_UpdateControls();
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveFavorites();
			break;
		}
		case FAV_SELECTION:
		{
			_UpdateControls();
			break;
		}
		case PASTE_ONLINE:
		{
			if (_CheckNetworkConnection() == true) {
				fThread = spawn_thread(_UploadClip, "clip_upload", B_LOW_PRIORITY, this);
				if (fThread >= B_OK)
					resume_thread(fThread);
			} else
				_PutClipboard(B_TRANSLATE("Online paste service not available"));

			// don't minimize/reset filter on SHIFT
			if ((modifiers() & (B_COMMAND_KEY | B_SHIFT_KEY)) != (B_COMMAND_KEY | B_SHIFT_KEY))
				PostMessage(MINIMIZE);

			break;
		}
		case INSERT_HISTORY:
		{
			int32 itemindex;
			message->FindInt32("index", &itemindex);
			if ((fHistory->IsEmpty()) || (itemindex < 0))
				break;

			Minimize(true);

			// When filtering is on, we're dealing with a temporary history list
			// that will be replaced with the backed-up fHistory when we _ResetFilter().
			// Keep watching the clipboard to make sure the inserted item is treated
			// like a new clip with _MakeItemUnique() and _MoveClipToTop().
			BString filter = fFilterControl->Text();
			if (filter == "")
				be_clipboard->StopWatching(this);

			ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(itemindex));
			BString text(item->GetClip());
			if (filter != "")
				_ResetFilter();

			_PutClipboard(text);

			if (fAutoPaste)
				_AutoPaste();

			if (filter == "")
				be_clipboard->StartWatching(this);

			_MoveClipToTop();
			break;
		}
		case INSERT_FAVORITE:
		{
			int32 itemindex;
			message->FindInt32("index", &itemindex);
			if ((fFavorites->IsEmpty()) || (itemindex < 0)
				|| (fFavorites->CountItems() <= itemindex))
				break;

			Minimize(true);

			FavItem* item = dynamic_cast<FavItem*>(fFavorites->ItemAt(itemindex));
			BString text(item->GetClip());
			_PutClipboard(text);
			if (fAutoPaste)
				_AutoPaste();
			break;
		}
		case CLEAR_FAVORITES:
		{
			fFavorites->MakeEmpty();
			_UpdateControls();
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveFavorites();
			break;
		}
		case FAV_DRAGGED:
		// until https://review.haiku-os.org/c/haiku/+/5800 is solved:
		{
			_SaveFavorites();
			break;
		}
		case CLEAR_HISTORY:
		{
			BString filter = fFilterControl->TextView()->Text();
			if (filter != "")
				_ResetFilter();

			fHistory->MakeEmpty();
			PostMessage(B_CLIPBOARD_CHANGED);
// save history with every new clip until
// https://review.haiku-os.org/c/haiku/+/5800 is solved
_SaveHistory();
			break;
		}
		case HELP:
		{
			_OpenHelp();
			break;
		}
		case UPDATE_SETTINGS:
		{
			int32 newValue;
			if (message->FindInt32("limit", &newValue) == B_OK) {
				if (fLimit >= newValue)
					_CropHistory(newValue);

				if (fLimit != newValue)
					fLimit = newValue;
			}

			if (message->FindInt32("autopaste", &newValue) == B_OK)
				fAutoPaste = newValue;

			if (message->FindInt32("fade", &newValue) == B_OK) {
				fMenuPauseFading->SetEnabled(newValue);
				if (newValue == 0) {
					fMenuPauseFading->SetMarked(false);
					SetTitle(B_TRANSLATE_SYSTEM_NAME("Clipdinger"));

					Settings* settings = my_app->GetSettings();
					if (settings->Lock()) {
						settings->SetFadePause(false);
						settings->Unlock();
					}
				}
			}
			_UpdateColors();
			break;
		}
		case FILTER_CLEAR:
		{
			_ResetFilter();
			break;
		}
		case FILTER_INPUT:
		{
			if (fBackup.IsEmpty())
				_BackupHistory();

			BString filter = fFilterControl->TextView()->Text();

			// avoid focus on fFilterControl, it eats e.g. cursor keys
			if (fFilterControl->TextView()->IsFocus()) {
				fHistory->MakeFocus(true);
				// a letter got entered in fFilterControl:
				// remove it, it'll arrive again from the KeyCatcher
				if (filter != "")
					filter.Truncate(filter.CountChars() - 1);
			}

			BString input;
			if (message->FindString("input", &input) == B_OK) {
				if (input == "BACKSPACE") {
					if (filter == "")
						break;
					filter.Truncate(filter.CountChars() - 1);
				} else
					filter.Append(input.String());

				fFilterControl->SetText(filter);
			}

			if (filter == "") {
				_ResetFilter();
				break;
			}
			_RestoreHistory();

			for (int32 i = fHistory->CountItems() - 1; i >= 0; i--) {
				ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(i));
				BString clip(item->GetClip());
				if (strcasestr(clip.String(), filter.String()) == NULL)
					fHistory->RemoveItem(i);
			}
			fHistory->Select(0);
			break;
		}
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}


void
MainWindow::WindowActivated(bool active)
{
	if (active)
		_UpdateColors();
}

// #pragma mark - Layout


void
MainWindow::_ResetFilter()
{
	fFilterControl->SetText("");

	_RestoreHistory();

	fHistory->Select(0);
	fBackup.MakeEmpty();
}


void
MainWindow::_BackupHistory()
{
	fBackup.MakeEmpty();
	for (int32 i = fHistory->CountItems() - 1; i >= 0; i--) {
		ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(i));
		fBackup.AddItem(item);
	}
}


void
MainWindow::_RestoreHistory()
{
	fHistory->MakeEmpty();
	for (int32 i = fBackup.CountItems() - 1; i >= 0; i--) {
		ClipItem* item = dynamic_cast<ClipItem*>(fBackup.ItemAt(i));
		fHistory->AddItem(item);
	}
}


void
MainWindow::_BuildLayout()
{
	// The menu
	BMenuBar* menuBar = new BMenuBar("menubar");
	BMenuItem* item;

	BMenu* menu = new BMenu("");
	item = new BMenuItem(B_TRANSLATE("Clipboard monitor"), new BMessage(CLIPMONITOR));
	item->SetTarget(be_app);
	menu->AddItem(item);
	item = new BMenuItem(B_TRANSLATE("Settings"), new BMessage(SETTINGS), ',');
	item->SetTarget(be_app);
	menu->AddItem(item);
	menu->AddSeparatorItem();
	item = new BMenuItem(B_TRANSLATE("Help"), new BMessage(HELP), 'H');
	menu->AddItem(item);
	item = new BMenuItem(B_TRANSLATE("About Clipdinger"), new BMessage(B_ABOUT_REQUESTED));
	menu->AddItem(item);
	item->SetTarget(be_app);
	menu->AddSeparatorItem();
	item = new BMenuItem(B_TRANSLATE("Minimize"), new BMessage(MINIMIZE), 'W');
	menu->AddItem(item);
	item = new BMenuItem(B_TRANSLATE("Quit"), new BMessage(DOQUIT), 'Q');
	menu->AddItem(item);

	menuBar->AddItem(new IconMenuItem(menu, NULL, kApplicationSignature, B_MINI_ICON));

	menu = new BMenu(B_TRANSLATE("Clip"));
	fMenuPaste = new BMenuItem(B_TRANSLATE("Paste online"), new BMessage(PASTE_ONLINE), 'P');
	menu->AddItem(fMenuPaste);
	BMessage* message = new BMessage(FAV_ADD);
	message->AddInt32("clipdinger_command", FAV_ADD);
	fMenuAdd = new BMenuItem(B_TRANSLATE("Add to favorites"), message, 'A');
	menu->AddItem(fMenuAdd);
	fMenuEdit = new BMenuItem(B_TRANSLATE("Edit title"), new BMessage(EDIT_TITLE), 'E');
	menu->AddItem(fMenuEdit);
	fMenuDelete = new BMenuItem(B_TRANSLATE("Remove"), new BMessage(DELETE));
	menu->AddItem(fMenuDelete);
	menuBar->AddItem(menu);

	menu = new BMenu(B_TRANSLATE("Lists"));
	item = new BMenuItem(B_TRANSLATE("Clear clip history"), new BMessage(CLEAR_HISTORY));
	menu->AddItem(item);
	fMenuClearFav = new BMenuItem(B_TRANSLATE("Clear favorites"), new BMessage(CLEAR_FAVORITES));
	menu->AddItem(fMenuClearFav);
	menu->AddSeparatorItem();
	fMenuPauseFading = new BMenuItem(B_TRANSLATE("Pause fading"), new BMessage(PAUSE), 'F');
	menu->AddItem(fMenuPauseFading);
	menuBar->AddItem(menu);

	// The lists
	fHistory = new ClipView("historyview");
	fHistory->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	fFavorites = new FavView("favoritesview");
	fFavorites->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	BScrollView* historyScrollView
		= new BScrollView("historyscroll", fHistory, B_WILL_DRAW, false, true);
	BScrollView* favoritesScrollView
		= new BScrollView("favoritesscroll", fFavorites, B_WILL_DRAW, false, true);

	fFilterControl = new BTextControl("filterbox", B_TRANSLATE("Filter:"), "", NULL);
	fFilterControl->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	BStringView* favoritesHeader = new BStringView("favorites", B_TRANSLATE("Favorites"));
	favoritesHeader->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	BFont font(be_bold_font);
	favoritesHeader->SetFont(&font);
	favoritesHeader->SetAlignment(B_ALIGN_CENTER);

	// The buttons
	fButtonUp = new BButton("up", B_TRANSLATE("Move up"), new BMessage(FAV_UP));
	fButtonUp->SetEnabled(false);

	fButtonDown = new BButton("down", B_TRANSLATE("Move down"), new BMessage(FAV_DOWN));
	fButtonDown->SetEnabled(false);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(menuBar)
		.AddSplit(B_HORIZONTAL, B_USE_SMALL_SPACING)
		.GetSplitView(&fMainSplitView)
		.AddGroup(B_VERTICAL)
			.Add(historyScrollView)
			.Add(fFilterControl)
			.End()
		.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
			.Add(favoritesHeader)
			.Add(favoritesScrollView)
			.AddGroup(B_HORIZONTAL, B_USE_SMALL_SPACING)
				.AddGlue()
				.Add(fButtonUp)
				.Add(fButtonDown)
				.AddGlue()
				.End()
			.End()
		.SetInsets(B_USE_SMALL_INSETS)
		.End();

	fHistory->SetInvocationMessage(new BMessage(INSERT_HISTORY));
	fHistory->SetViewColor(B_TRANSPARENT_COLOR);
	fFavorites->SetInvocationMessage(new BMessage(INSERT_FAVORITE));
	fFavorites->SetSelectionMessage(new BMessage(FAV_SELECTION));
	fFavorites->SetViewColor(B_TRANSPARENT_COLOR);

	AddShortcut('P', B_SHIFT_KEY, new BMessage(PASTE_ONLINE));
}


void
MainWindow::_SetSplitview()
{
	float leftWeight = 0;
	float rightWeight = 0;
	bool leftCollapse = false;
	bool rightCollapse = false;
	Settings* settings = my_app->GetSettings();
	if (settings->Lock()) {
		settings->GetSplitWeight(leftWeight, rightWeight);
		settings->GetSplitCollapse(leftCollapse, rightCollapse);
		settings->Unlock();
	}
	fMainSplitView->SetItemWeight(0, leftWeight, false);
	fMainSplitView->SetItemCollapsed(0, leftCollapse);

	fMainSplitView->SetItemWeight(1, rightWeight, true);
	fMainSplitView->SetItemCollapsed(1, rightCollapse);
}


// #pragma mark - Load & Save Lists


void
MainWindow::_SaveHistory()
{
	BPath path;
	BMessage msg;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) < B_OK)
		return;
	status_t ret = path.Append(kSettingsFolder);

	if (ret == B_OK)
		ret = create_directory(path.Path(), 0777);

	if (ret == B_OK)
		path.Append(kHistoryFile);

	if (ret == B_OK) {
		BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		ret = file.InitCheck();

		if (ret == B_OK) {
			for (int i = fHistory->CountItems() - 1; i >= 0; i--) {
				ClipItem* sItem = dynamic_cast<ClipItem*>(fHistory->ItemAt(i));

				BString clip(sItem->GetClip());
				BString title(sItem->GetTitle());
				if (title == clip)
					title = "";
				BString path(sItem->GetOrigin());
				bigtime_t added(sItem->GetTimeAdded());
				msg.AddString("clip", clip.String());
				msg.AddString("title", title.String());
				msg.AddString("origin", path.String());
				msg.AddInt64("time", added);
			}
			msg.AddInt64("quittime", real_time_clock());
			msg.Flatten(&file);
		}
	}
}


void
MainWindow::_LoadHistory()
{
	BPath path;
	BMessage msg;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(kSettingsFolder);
		if (ret == B_OK) {
			path.Append(kHistoryFile);
			BFile file(path.Path(), B_READ_ONLY);

			if (file.InitCheck() != B_OK || (msg.Unflatten(&file) != B_OK))
				return;
			else {
				fHistory->MakeEmpty();

				BString clip;
				BString title;
				BString path;
				int32 old_added = 0; // used int32 pre v.0.5.5,
				int32 old_quittime = 0; // read old history files too.
				bigtime_t added = 0;
				bigtime_t quittime = 0;
				bigtime_t since = 0;

				if (msg.FindInt32("quittime", &old_quittime) == B_OK)
					quittime = (int64) old_quittime;
				else if (msg.FindInt64("quittime", &quittime) != B_OK)
					quittime = real_time_clock();

				int32 i = 0;
				while ((msg.FindString("clip", i, &clip) == B_OK)
					&& (msg.FindString("origin", i, &path) == B_OK)
					&& ((msg.FindInt32("time", i, &old_added) == B_OK)
						|| (msg.FindInt64("time", i, &added) == B_OK))) {

					if (msg.FindString("title", i, &title) != B_OK)
						title = ""; // if there's no title found (pre v1.0)

					if (added == 0)
						added = (int64) old_added;

					since = added + (fLaunchTime - quittime);
					_AddClip(clip, title, path, added, since);
					i++;
				}
				fHistory->AdjustColors();
			}
		}
	}
}


void
MainWindow::_SaveFavorites()
{
	BPath path;
	BMessage msg;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) < B_OK)
		return;
	status_t ret = path.Append(kSettingsFolder);

	if (ret == B_OK)
		ret = create_directory(path.Path(), 0777);

	if (ret == B_OK)
		path.Append(kFavoritesFile);

	if (ret == B_OK) {
		BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		ret = file.InitCheck();

		if (ret == B_OK) {
			for (int i = 0; i < fFavorites->CountItems(); i++) {
				FavItem* sItem = dynamic_cast<FavItem*>(fFavorites->ItemAt(i));

				BString clip(sItem->GetClip());
				BString title(sItem->GetTitle());
				if (title == clip)
					title = "";
				msg.AddString("clip", clip.String());
				msg.AddString("title", title.String());
			}
			msg.Flatten(&file);
		}
	}
}


void
MainWindow::_LoadFavorites()
{
	BPath path;
	BMessage msg;

	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(kSettingsFolder);
		if (ret == B_OK) {
			path.Append(kFavoritesFile);
			BFile file(path.Path(), B_READ_ONLY);

			if (file.InitCheck() != B_OK || (msg.Unflatten(&file) != B_OK))
				return;
			else {
				BString clip;
				BString title;
				int32 i = 0;
				while (msg.FindString("clip", i, &clip) == B_OK
					&& msg.FindString("title", i, &title) == B_OK) {
					fFavorites->AddItem(new FavItem(clip, title, i), i);
					i++;
				}
			}
		}
	}
}


void
MainWindow::_OpenHelp()
{
	BPathFinder pathFinder;
	BStringList paths;
	BPath path;
	BEntry entry;

	status_t error = pathFinder.FindPaths(B_FIND_PATH_DOCUMENTATION_DIRECTORY,
		"packages/clipdinger", paths);

	for (int i = 0; i < paths.CountStrings(); ++i) {
		if (error == B_OK && path.SetTo(paths.StringAt(i)) == B_OK
			&& path.Append("ReadMe.html") == B_OK) {
			entry = path.Path();
			entry_ref ref;
			entry.GetRef(&ref);
			be_roster->Launch(&ref);
		}
	}
}


// #pragma mark - Clips etc.

void
MainWindow::_AddClip(BString clip, BString title, BString path, bigtime_t added, bigtime_t since)
{
	if (fHistory->CountItems() > fLimit - 1)
		fHistory->RemoveItem(fHistory->LastItem());

	fHistory->AddItem(new ClipItem(clip, title, path, added, since), 0);
}


void
MainWindow::_MakeItemUnique(BString clip)
{
	if (fHistory->IsEmpty())
		return;

	for (int i = 0; i < fHistory->CountItems(); i++) {
		ClipItem* sItem = dynamic_cast<ClipItem*>(fHistory->ItemAt(i));
		BString* listItem = new BString(sItem->GetClip());

		if (clip.Compare(*listItem) == 0)
			fHistory->RemoveItem(i);
	}
}


void
MainWindow::_MoveClipToTop()
{
	fHistory->MoveItem(fHistory->CurrentSelection(), 0);
	fHistory->Select(0);

	bigtime_t time(real_time_clock());
	ClipItem* item = dynamic_cast<ClipItem*>(fHistory->ItemAt(0));
	item->SetTimeAdded(time);
}


void
MainWindow::_CropHistory(int32 limit)
{
	if (limit < fLimit) {
		if (fHistory->CountItems() > limit) {
			int count = fHistory->CountItems() - limit;
			if (limit == 0)
				limit = 1;
			fHistory->RemoveItems(limit, count);
		}
	}
}


bool
MainWindow::_CheckNetworkConnection()
{
	BNetworkRoster& roster = BNetworkRoster::Default();
	BNetworkInterface interface;
	uint32 cookie = 0;
	while (roster.GetNextInterface(&cookie, interface) == B_OK) {
		uint32 flags = interface.Flags();
		if ((flags & IFF_LOOPBACK) == 0 && (flags & (IFF_UP | IFF_LINK)) == (IFF_UP | IFF_LINK)) {
			return true;
		}
	}
	// No network connection detected, cannot continue
	return false;
}


status_t
MainWindow::_UploadClip(void* self)
{
	MainWindow* window = reinterpret_cast<MainWindow*>(self);
	BString text;

	if (window->GetHistoryActiveFlag() && !window->fHistory->IsEmpty()) {
		int32 index = window->fHistory->CurrentSelection();
		if (index < 0)
			return B_ERROR;

		ClipItem* item = dynamic_cast<ClipItem*>(window->fHistory->ItemAt(index));
		text = item->GetClip();

	} else if (!window->GetHistoryActiveFlag() && !window->fFavorites->IsEmpty()) {
		int32 index = window->fFavorites->CurrentSelection();
		if (index < 0)
			return B_ERROR;

		FavItem* item = dynamic_cast<FavItem*>(window->fFavorites->ItemAt(index));
		text = item->GetClip();

	} else
		return B_ERROR;

	BPath tempPath;
	status_t ret = find_directory(B_SYSTEM_TEMP_DIRECTORY, &tempPath);

	if (ret == B_OK) {
		ret = tempPath.Append("clip_upload");

		if (ret == B_OK) {
			BFile tempFile(tempPath.Path(), B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
			ret = tempFile.InitCheck();

			if (ret == B_OK) {
				ssize_t written = tempFile.Write(text.String(), text.Length());

				if (written != text.Length())
					ret = (status_t) written;
			}
		}
	}

	if (ret != B_OK) {
		BString error(B_TRANSLATE(
			"Failed to upload the clip.\n"
			"The temporary file could not be written to disk."));
		BAlert* alert = new BAlert(B_TRANSLATE("Upload error"), error, B_TRANSLATE("OK"));
		alert->Go();
		return B_ERROR;
	}

	BString title = window->Title();
	BString uploadTitle = title;
	window->SetTitle(uploadTitle << " (" << B_TRANSLATE("Upload") << B_UTF8_ELLIPSIS << ")");

	BString command(
		"URL=$(curl -F 'file=@'\"%FILE%\" https://0x0.st) ; "
		"clipboard -c \"$URL\" ; "
		"exit");
	command.ReplaceFirst("%FILE%", tempPath.Path());
	system(command.String());

	window->SetTitle(title);

	BEntry entry(tempPath.Path());
	if (entry.Exists())
		entry.Remove();

	return B_OK;
}

// #pragma mark - Clipboard


BString
MainWindow::_GetClipboard()
{
	const char* text = NULL;
	ssize_t textLen = 0;
	BMessage* clipboard = (BMessage*)NULL;

	if (be_clipboard->Lock()) {
		if ((clipboard = be_clipboard->Data()))
			clipboard->FindData("text/plain", B_MIME_TYPE, (const void**)&text, &textLen);
		be_clipboard->Unlock();
	}
	BString clip(text, textLen);
	return clip;
}


void
MainWindow::_PutClipboard(BString text)
{
	ssize_t textLen = text.Length();
	BMessage* clip = (BMessage*)NULL;

	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		if ((clip = be_clipboard->Data())) {
			clip->AddData("text/plain", B_MIME_TYPE, text.String(), textLen);
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
}


// #pragma mark - Settings & Display


void
MainWindow::_AutoPaste()
{
	port_id port = find_port(OUTPUT_PORT_NAME);
	if (port != B_NAME_NOT_FOUND)
		write_port(port, 'CtSV', NULL, 0);
}


void
MainWindow::_UpdateControls()
{
	bool active = GetHistoryActiveFlag();
	int32 selection = fFavorites->CurrentSelection();
	int32 count = fFavorites->CountItems();

	if (active == false && count == 0) {
		fMenuDelete->SetEnabled(false);
		fMenuEdit->SetEnabled(false);
		fMenuPaste->SetEnabled(false);
	}
	if (active) {
		fMenuDelete->SetEnabled(true);
		fMenuEdit->SetEnabled(true);
		fMenuPaste->SetEnabled(true);
	}
	fMenuClearFav->SetEnabled((count > 0) ? true : false);

	if (selection < 0)
		count = -1;

	fButtonUp->SetEnabled((count > 1 && selection > 0 && !active) ? true : false);
	fButtonDown->SetEnabled((count > 1 && selection < count - 1 && !active) ? true : false);
	fMenuAdd->SetEnabled((active == true) ? true : false);
}


void
MainWindow::_UpdateColors()
{
	BMessenger messenger(fHistory);
	BMessage message(ADJUSTCOLORS);
	messenger.SendMessage(&message);
}


bool
MainWindow::GetHistoryActiveFlag()
{
	return fHistoryActiveFlag;
}


void
MainWindow::SetHistoryActiveFlag(bool flag)
{
	fHistoryActiveFlag = flag;
	fHistory->Invalidate();
	fFavorites->Invalidate();
	_UpdateControls();
}
