/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "MainWindow.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>

#include <algorithm>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MainWindow"


MainWindow::MainWindow(BRect frame)
	:
	BWindow(frame, "Clipdinger", B_TITLED_WINDOW, B_AUTO_UPDATE_SIZE_LIMITS,
		B_ALL_WORKSPACES),
		fLimit(50)
{
	_BuildLayout();
	be_clipboard->StartWatching(this);
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
		new BMessage(CLEAR_HISTORY),'H');	
	menu->AddItem(item);
	submenu = new BMenu(B_TRANSLATE("Limit history"));
	fLimit25 = new BMenuItem(B_TRANSLATE("25 entries"),
		new BMessage(ENTRIES_25));
	submenu->AddItem(fLimit25);
	fLimit50 = new BMenuItem(B_TRANSLATE("50 entries"),
		new BMessage(ENTRIES_50));
	submenu->AddItem(fLimit50);
	fLimit50->SetMarked(true);
	fLimit100 = new BMenuItem(B_TRANSLATE("100 entries"),
		new BMessage(ENTRIES_100));
	submenu->AddItem(fLimit100);
	fLimit200 = new BMenuItem(B_TRANSLATE("200 entries"),
		new BMessage(ENTRIES_200));
	submenu->AddItem(fLimit200);
	menu->AddItem(submenu);
	menuBar->AddItem(menu);

	// The lists
	fClipList = new BListView("cliplist");
	fClipList->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
//	fFavoriteList = new BListView("favoritelist");
//	fFavoriteList->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	fClipScrollView = new BScrollView("clipscroll", fClipList,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);
/*	fFavoriteScrollView = new BScrollView("favoritescroll", fFavoriteList,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);

	BStringView* favoriteHeader = new BStringView("title",
		B_TRANSLATE("Saved favourites:"));
	favoriteHeader->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
*/
	// do the layouting				
	float padding = be_control_look->DefaultItemSpacing();	
	BSplitView *v = 
		BLayoutBuilder::Split<>(B_VERTICAL)
			.AddGroup(B_VERTICAL)
				.Add(fClipScrollView)
			.End()
/*			.AddGroup(B_VERTICAL, padding / 2)
				.Add(favoriteHeader)
				.Add(fFavoriteScrollView)
			.End()
*/		.SetInsets(padding, 0, padding, padding)
		.View();

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(menuBar)
		.Add(v);

	fClipList->MakeFocus(true);
	fClipList->SetInvocationMessage(new BMessage(MSG_INSERT_CLIP));
//	fFavoriteList->SetInvocationMessage(new BMessage(MSG_INSERT_FAVORITE));
}


bool
MainWindow::QuitRequested()
{
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
			fClipList->DeselectAll();
			BString clipboardString(GetClipboard());
			if (IsItemUnique(clipboardString))
				AddClip(clipboardString);
			fClipList->Select(0);
			break;
		}
		case CLEAR_HISTORY:
		{
			fClipList->MakeEmpty();
			PostMessage(B_CLIPBOARD_CHANGED);
			break;
		}
		case ENTRIES_25:
		{
			fLimit = 25;
			fLimit25->SetMarked(true);
			fLimit50->SetMarked(false);
			fLimit100->SetMarked(false);
			fLimit200->SetMarked(false);

			if (fClipList->CountItems() > fLimit)
				CropHistory(fLimit);
			break;
		}
		case ENTRIES_50:
		{
			fLimit = 50;
			fLimit25->SetMarked(false);
			fLimit50->SetMarked(true);
			fLimit100->SetMarked(false);
			fLimit200->SetMarked(false);

			if (fClipList->CountItems() > fLimit)
				CropHistory(fLimit);
			break;
		}
		case ENTRIES_100:
		{
			fLimit = 100;
			fLimit25->SetMarked(false);
			fLimit50->SetMarked(false);
			fLimit100->SetMarked(true);
			fLimit200->SetMarked(false);

			if (fClipList->CountItems() > fLimit)
				CropHistory(fLimit);
			break;
		}
		case ENTRIES_200:
		{
			fLimit = 200;
			fLimit25->SetMarked(false);
			fLimit50->SetMarked(false);
			fLimit100->SetMarked(false);
			fLimit200->SetMarked(true);

			if (fClipList->CountItems() > fLimit)
				CropHistory(fLimit);
			break;
		}
		case MSG_INSERT_CLIP:
		{
			if (fClipList->IsEmpty())
				break;
			Minimize(true);
			PutClipboard(fClipList);
			break;
		}
		case MSG_INSERT_FAVORITE:
		{
			if (fFavoriteList->IsEmpty())
				break;
			Minimize(true);
			PutClipboard(fFavoriteList);
			break;
		}		
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
	if (fClipList->IsEmpty())
		return true;

	for (int i = 0; i < fClipList->CountItems(); i++)
	{
		BStringItem *sItem = dynamic_cast<BStringItem *> (fClipList->ItemAt(i));
		BString *listItem = new BString(sItem->Text());

		if (clipboardString.Compare(*listItem, std::min(clipboardString.Length(),
				listItem->Length())) == 0) {
			fClipList->MoveItem(i, 0);
			return false;
		}
	}
	return true;
}


void
MainWindow::AddClip(BString clipboardString)
{
	if (fClipList->CountItems() > fLimit - 1)
		fClipList->RemoveItem(fClipList->LastItem());
	fClipList->AddItem(new BStringItem(clipboardString), 0);
	return;
}


void
MainWindow::CropHistory(int limit)
{
	int count = fClipList->CountItems() - limit;
	fClipList->RemoveItems(limit, count);
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
	BStringItem *item = dynamic_cast<BStringItem *> (list->ItemAt(index));
	BString text(item->Text());
		
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
