/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Catalog.h>
#include <ControlLook.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <LayoutBuilder.h>
#include <Roster.h>
#include <Screen.h>

#include <algorithm>

#include "App.h"
#include "ClipItem.h"
#include "FavItem.h"
#include "Constants.h"
#include "MainWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"


MainWindow::MainWindow(BRect frame)
	:
	BWindow(BRect(), B_TRANSLATE_SYSTEM_NAME("Clipdinger"), B_TITLED_WINDOW,
		B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS,
		B_ALL_WORKSPACES)
{
	_BuildLayout();

	if (frame == BRect(-1, -1, -1, -1)) {
		CenterOnScreen();
		ResizeTo(300, 400);
	} else {
		MoveTo(frame.LeftTop());
		ResizeTo(frame.right - frame.left, frame.bottom - frame.top);

		// make sure window is on screen
		BScreen screen(this);
		if (!screen.Frame().InsetByCopy(10, 10).Intersects(Frame()))
			CenterOnScreen();
	}

	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		fLimit = settings->GetLimit();
		settings->Unlock();
	}
	fLaunchTime = real_time_clock();

	_LoadHistory();
	_LoadFavorites();

	if (!fHistory->IsEmpty())
		fHistory->Select(0);

	if (GetClipboard() == "") {
		if (!fHistory->IsEmpty()) {
			ClipItem* item = dynamic_cast<ClipItem *> (fHistory->ItemAt(0));
			BString text(item->GetClip());
			PutClipboard(text);
		}
	}
	be_clipboard->StartWatching(this);
}


MainWindow::~MainWindow()
{
}


bool
MainWindow::QuitRequested()
{
	_SaveHistory();
	_SaveFavorites();

	ClipdingerSettings* settings = my_app->Settings();
	if (settings->Lock()) {
		settings->SetWindowPosition(ConvertToScreen(Bounds()));
		settings->Unlock();
	}

	if (fSettingsWindow) {
		BMessenger messenger(fSettingsWindow);
		if (messenger.IsValid() && messenger.LockTarget())
			fSettingsWindow->Quit();
	}

	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::_BuildLayout()
{
	// The menu
	BMenuBar* menuBar = new BMenuBar("menubar");
	BMenu* menu;
	BMenu* submenu;
	BMenuItem* item;
	
	menu = new BMenu(B_TRANSLATE("App"));
	item = new BMenuItem(B_TRANSLATE("About Clipdinger"),
		new BMessage(B_ABOUT_REQUESTED));
	menu->AddItem(item);
	item->SetTarget(be_app);
	item = new BMenuItem(B_TRANSLATE("Help"),
		new BMessage(HELP));
	menu->AddItem(item);
	item = new BMenuItem(B_TRANSLATE("Minimize"),
		new BMessage(ESCAPE), 'W');
	menu->AddItem(item);
	item = new BMenuItem(B_TRANSLATE("Quit"),
		new BMessage(B_QUIT_REQUESTED), 'Q');
	menu->AddItem(item);	
	menuBar->AddItem(menu);

	menu = new BMenu(B_TRANSLATE("History"));

	item = new BMenuItem(B_TRANSLATE("Clear history"),
		new BMessage(CLEAR_HISTORY));
	menu->AddItem(item);

	item = new BMenuItem(B_TRANSLATE("Settings" B_UTF8_ELLIPSIS),
		new BMessage(SETTINGS));
	menu->AddItem(item);
	menuBar->AddItem(menu);

	// The lists
	fHistory = new ClipView("history");
	fHistory->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	fFavorites = new FavView("favorites");
	fFavorites->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	fHistoryScrollView = new BScrollView("historyscroll", fHistory,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);
	fFavoriteScrollView = new BScrollView("favoritescroll", fFavorites,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);

	BStringView* favoriteHeader = new BStringView("title",
		B_TRANSLATE("Saved favourites:"));
	favoriteHeader->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	// do the layouting				
	static const float spacing = be_control_look->DefaultItemSpacing() / 2;
	BSplitView* v =
		BLayoutBuilder::Split<>(B_HORIZONTAL)
			.AddGroup(B_VERTICAL)
				.Add(fHistoryScrollView)
			.End()
			.AddGroup(B_VERTICAL, spacing / 2)
				.Add(favoriteHeader)
				.Add(fFavoriteScrollView)
			.End()
		.SetInsets(spacing)
		.View();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(menuBar)
		.Add(v);

	fHistory->MakeFocus(true);
	fHistory->SetInvocationMessage(new BMessage(INSERT_HISTORY));
	fHistory->SetViewColor(B_TRANSPARENT_COLOR);
	fFavorites->SetInvocationMessage(new BMessage(INSERT_FAVORITE));
	fFavorites->SetViewColor(B_TRANSPARENT_COLOR);
}


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
			for (int i = fHistory->CountItems() - 1; i >= 0 ; i--)
			{
				ClipItem *sItem = dynamic_cast<ClipItem *>
					(fHistory->ItemAt(i));

				BString clip(sItem->GetClip());
				BString path(sItem->GetOrigin());
				int32 time(sItem->GetTimeAdded());
				msg.AddString("clip", clip.String());
				msg.AddString("origin", path.String());
				msg.AddInt32("time", time);
			}
			msg.AddInt32("quittime", real_time_clock());
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
				BString clip;
				BString path;
				int32 time;
				int32 quittime;
				if (msg.FindInt32("quittime", &quittime) != B_OK) {
					printf("no quit time.\n");
					int32 quittime(real_time_clock());
				}
				int32 i = 0;
				while ((msg.FindString("clip", i, &clip) == B_OK) &&
						(msg.FindString("origin", i, &path) == B_OK) &&
						(msg.FindInt32("time", i, &time) == B_OK)) {
					time = time + (fLaunchTime - quittime);
					AddClip(clip, path, time);
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
		path.Append(kFavoriteFile);

	if (ret == B_OK) {
		BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		ret = file.InitCheck();

		if (ret == B_OK) {
			for (int i = 0; i < fFavorites->CountItems(); i++)
			{
				FavItem *sItem = dynamic_cast<FavItem *>
					(fFavorites->ItemAt(i));

				BString clip(sItem->GetClip());
				msg.AddString("clip", clip.String());
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
			path.Append(kFavoriteFile);
			BFile file(path.Path(), B_READ_ONLY);

			if (file.InitCheck() != B_OK || (msg.Unflatten(&file) != B_OK))
				return;
			else {
				BString clip;
				BString path;
				int32 i = 0;
				while (msg.FindString("clip", i, &clip) == B_OK) {
					fFavorites->AddItem(new FavItem(clip, i), i);
					i++;
				}
			}
		}
	}
}


void
MainWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case B_CLIPBOARD_CHANGED:
		{
			BString clip(GetClipboard());
			if (clip.Length() == 0)
				break;
			fHistory->DeselectAll();

			app_info info;
			BPath path;
			be_roster->GetActiveAppInfo(&info);
			BEntry entry(&info.ref);
			entry.GetPath(&path);

			MakeItemUnique(clip);
			int32 time(real_time_clock());
			AddClip(clip, path.Path(), time);

			fHistory->Select(0);
			break;
		}
		case ESCAPE:
		{
			Minimize(true);
			break;
		}
		case DELETE:
		{
			if (!fHistory->IsEmpty());
				fHistory->RemoveItem(fHistory->CurrentSelection());
			break;
		}
		case ADD_FAV:
		{
			AddFav();
			break;
		}
		case DELETE_FAV:
		{
			if (!fFavorites->IsEmpty());
				fFavorites->RemoveItem(fFavorites->CurrentSelection());
			break;
		}
		case HELP:
		{
			app_info info;
			BPath path;
			be_roster->GetActiveAppInfo(&info);
			BEntry entry(&info.ref);

			entry.GetPath(&path);
			path.GetParent(&path);
			path.Append("ReadMe.html");

			entry = path.Path();
			entry_ref ref;
			entry.GetRef(&ref);
			be_roster->Launch(&ref);
			break;
		}
		case CLEAR_HISTORY:
		{
			fHistory->MakeEmpty();
			PostMessage(B_CLIPBOARD_CHANGED);
			break;
		}
		case SETTINGS:
		{
			fSettingsWindow = new SettingsWindow(Frame());
			fSettingsWindow->Show();
			break;
		}
		case INSERT_HISTORY:
		{
			int32 itemindex;
			message->FindInt32("index", &itemindex);
			if ((fHistory->IsEmpty()) || (itemindex < 0))
				break;

			Minimize(true);
			be_clipboard->StopWatching(this);

			ClipItem* item = dynamic_cast<ClipItem *> (fHistory->ItemAt(itemindex));
			BString text(item->GetClip());
			PutClipboard(text);
			if (fAutoPaste)
				AutoPaste();
			MoveClipToTop();
			UpdateColors();
			be_clipboard->StartWatching(this);
			break;
		}
		case INSERT_FAVORITE:
		{
			int32 itemindex;
			message->FindInt32("index", &itemindex);
			if ((fFavorites->IsEmpty()) || (itemindex < 0))
				break;

			Minimize(true);
			be_clipboard->StopWatching(this);

			FavItem* item = dynamic_cast<FavItem *> (fFavorites->ItemAt(itemindex));
			BString text(item->GetClip());
			PutClipboard(text);
			if (fAutoPaste)
				AutoPaste();
			be_clipboard->StartWatching(this);
			break;
		}
		case UPDATE_SETTINGS:
		{
			int32 newValue;
			if (message->FindInt32("limit", &newValue) == B_OK) {
				if (fLimit >= newValue)
					CropHistory(newValue);

				if (fLimit != newValue)
					fLimit = newValue;

				BMessenger messenger(fHistory);
				BMessage message(ADJUSTCOLORS);
				messenger.SendMessage(&message);
			}
			if (message->FindInt32("autopaste", &newValue) == B_OK)
				fAutoPaste = newValue;
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
MainWindow::MakeItemUnique(BString clip)
{
	if (fHistory->IsEmpty())
		return;

	for (int i = 0; i < fHistory->CountItems(); i++) {
		ClipItem *sItem =
			dynamic_cast<ClipItem *> (fHistory->ItemAt(i));
		BString *listItem = new BString(sItem->GetClip());

		if (clip.Compare(*listItem) == 0)
			fHistory->RemoveItem(i);
	}
}


void
MainWindow::AddClip(BString clip, BString path, int32 time)
{
	if (fHistory->CountItems() > fLimit - 1)
		fHistory->RemoveItem(fHistory->LastItem());

	fHistory->AddItem(new ClipItem(clip, path, time), 0);
}


void
MainWindow::AddFav()
{
	int index = fHistory->CurrentSelection();
	if (index < 0)
		return;

	ClipItem *item = dynamic_cast<ClipItem *> (fHistory->ItemAt(index));
	BString clip(item->GetClip());

	int32 lastitem = fFavorites->CountItems();
	fFavorites->AddItem(new FavItem(clip, lastitem), lastitem);
}


void
MainWindow::CropHistory(int32 limit)
{
	if (limit < fLimit) {
		if (fHistory->CountItems() > limit) {
			int count = fHistory->CountItems() - limit - 1;
			if (limit == 0)
				limit = 1;
			fHistory->RemoveItems(limit, count);
		}
	}
}	


BString
MainWindow::GetClipboard()
{
	const char* text;
	ssize_t textLen;
	BMessage* clipboard;
	if (be_clipboard->Lock()) {
		if ((clipboard = be_clipboard->Data()))
			clipboard->FindData("text/plain", B_MIME_TYPE,
				(const void **)&text, &textLen);
		be_clipboard->Unlock();
	}
	BString clip(text, textLen);
	return clip;
}


void
MainWindow::PutClipboard(BString text)
{
	ssize_t textLen = text.Length();
	BMessage* clip = (BMessage *)NULL;
	
	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		if (clip = be_clipboard->Data()) {
			clip->AddData("text/plain", B_MIME_TYPE, text.String(), textLen);
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
}


void
MainWindow::AutoPaste()
{
	port_id port = find_port(OUTPUT_PORT_NAME);
	if (port != B_NAME_NOT_FOUND)
		write_port(port, 'CtSV', NULL, 0);
}


void
MainWindow::MoveClipToTop()
{
	fHistory->MoveItem(fHistory->CurrentSelection(), 0);
	fHistory->Select(0);

	int32 time(real_time_clock());
	ClipItem* item = dynamic_cast<ClipItem *> (fHistory->ItemAt(0));
	item->SetTimeAdded(time);
}


void
MainWindow::UpdateColors()
{
	BMessenger messenger(fHistory);
	BMessage message(ADJUSTCOLORS);
	messenger.SendMessage(&message);
}
