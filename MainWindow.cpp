/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "KeyFilter.h"
#include "ClipListItem.h"
#include "MainWindow.h"
#include "Settings.h"
#include "SettingsWindow.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <LayoutBuilder.h>
#include <Screen.h>

#include <algorithm>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"


MainWindow::MainWindow()
	:
	BWindow(BRect(), B_TRANSLATE_SYSTEM_NAME("Clipdinger"), B_TITLED_WINDOW,
		B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS,
		B_ALL_WORKSPACES)
{
	_BuildLayout();

	if (fSettings.WindowPosition() == BRect(-1, -1, -1, -1)) {
		CenterOnScreen();
		ResizeTo(300, 400);
	} else {
		BRect frame = fSettings.WindowPosition();
		MoveTo(frame.LeftTop());
		ResizeTo(frame.right - frame.left, frame.bottom - frame.top);

		// make sure window is on screen
		BScreen screen(this);
		if (!screen.Frame().InsetByCopy(10, 10).Intersects(Frame()))
			CenterOnScreen();
	}

	if (fSettings.Limit())
		fLimit = fSettings.Limit();

	_LoadHistory();

	be_clipboard->StartWatching(this);

	if (GetClipboard() != "")
		PostMessage(B_CLIPBOARD_CHANGED);

	AddCommonFilter(new KeyFilter);
}


MainWindow::~MainWindow()
{
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
	item = new BMenuItem(B_TRANSLATE("Quit"),
		new BMessage(B_QUIT_REQUESTED), 'Q');
	menu->AddItem(item);	
	menuBar->AddItem(menu);

	menu = new BMenu(B_TRANSLATE("History"));

	item = new BMenuItem(B_TRANSLATE("Clear history"),
		new BMessage(msgCLEAR_HISTORY));
	menu->AddItem(item);

	item = new BMenuItem(B_TRANSLATE("Settings" B_UTF8_ELLIPSIS),
		new BMessage(msgSETTINGS));
	menu->AddItem(item);
	menuBar->AddItem(menu);

	// The lists
	fHistory = new ClipListView("history");
	fHistory->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
//	fFavorites = new BListView("favorites");
//	fFavorites->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	fHistoryScrollView = new BScrollView("historyscroll", fHistory,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);
/*	fFavoriteScrollView = new BScrollView("favoritescroll", fFavorites,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);

	BStringView* favoriteHeader = new BStringView("title",
		B_TRANSLATE("Saved favourites:"));
	favoriteHeader->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
*/
	// do the layouting				
	const float padding = be_control_look->DefaultItemSpacing() / 2;
	BSplitView *v = 
		BLayoutBuilder::Split<>(B_VERTICAL)
			.AddGroup(B_VERTICAL)
				.Add(fHistoryScrollView)
			.End()
/*			.AddGroup(B_VERTICAL, padding / 2)
				.Add(favoriteHeader)
				.Add(fFavoriteScrollView)
			.End()
*/		.SetInsets(padding)
		.View();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(menuBar)
		.Add(v);

	fHistory->MakeFocus(true);
	fHistory->SetInvocationMessage(new BMessage(msgINSERT_HISTORY));
//	fFavorites->SetInvocationMessage(new BMessage(msgINSERT_FAVORITE));
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
				ClipListItem *sItem = dynamic_cast<ClipListItem *>
					(fHistory->ItemAt(i));

				BString *clip = new BString(sItem->GetClip());
				msg.AddString("clip", *clip);
			}
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
				int32 i = 0;
				while (msg.FindString("clip", i++, &clip) == B_OK)
					AddClip(clip);
			}
		}
	}
}


bool
MainWindow::QuitRequested()
{
	_SaveHistory();

	fSettings.SetLimit(fLimit);
	fSettings.SetWindowPosition(ConvertToScreen(Bounds()));

	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
MainWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case B_CLIPBOARD_CHANGED:
		{
			BString clipboardString(GetClipboard());
			if (clipboardString.Length() == 0)
				break;
			fHistory->DeselectAll();
			if (IsItemUnique(clipboardString))
				AddClip(clipboardString);
			fHistory->Select(0);
			break;
		}
		case msgESCAPE:
		{
			Minimize(true);
			break;
		}
		case msgDELETE:
		{
			if (!fHistory->IsEmpty());
				fHistory->RemoveItem(fHistory->CurrentSelection());
			break;
		}
		case msgCLEAR_HISTORY:
		{
			fHistory->MakeEmpty();
			PostMessage(B_CLIPBOARD_CHANGED);
			break;
		}
		case msgSETTINGS:
		{
			BRect frame = Frame();
			BWindow* settingswindow = new SettingsWindow(fLimit, frame);
			settingswindow->Show();
			break;
		}
		case msgINSERT_HISTORY:
		{
			if (fHistory->IsEmpty())
				break;
			Minimize(true);
			PutClipboard(fHistory);
			break;
		}
//		case msgINSERT_FAVORITE:
//		{
//			if (fFavorites->IsEmpty())
//				break;
//			Minimize(true);
//			PutClipboard(fFavorites);
//			break;
//		}
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}


bool
MainWindow::IsItemUnique(BString clipboardString)
{
	if (fHistory->IsEmpty())
		return true;

	for (int i = 0; i < fHistory->CountItems(); i++)
	{
		ClipListItem *sItem = dynamic_cast<ClipListItem *> (fHistory->ItemAt(i));
		BString *listItem = new BString(sItem->GetClip());

		if (clipboardString.Compare(*listItem) == 0) {
			fHistory->MoveItem(i, 0);
			return false;
		}
	}
	return true;
}


void
MainWindow::AddClip(BString clipboardString)
{
	if (fHistory->CountItems() > fLimit - 1)
		fHistory->RemoveItem(fHistory->LastItem());

	fHistory->AddItem(new ClipListItem(clipboardString), 0);
	return;
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
	return;
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
	BString clipboardString(text, textLen);
	return clipboardString;
}


void
MainWindow::PutClipboard(BListView* list)
{
	BMessage* clip = (BMessage *)NULL;
	int index = list->CurrentSelection();
	ClipListItem *item = dynamic_cast<ClipListItem *> (list->ItemAt(index));
	BString text(item->GetClip());
		
	ssize_t textLen = text.Length();
	
	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		if (clip = be_clipboard->Data()) {
			clip->AddData("text/plain", B_MIME_TYPE, text.String(), textLen);
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();

		port_id port = find_port(OUTPUT_PORT_NAME);
		if (port != B_NAME_NOT_FOUND)
			write_port(port, 'CtSV', NULL, 0);
	}
	return;
}


void
MainWindow::UpdatedSettings(int32 limit)
{
	if (fLimit >= limit)
		CropHistory(limit);

	if (fLimit != limit)
		fLimit = limit;

	return;
}
