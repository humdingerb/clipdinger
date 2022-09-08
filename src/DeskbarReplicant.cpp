/*
 * Copyright 2009-2021, Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Weirauch, dev@m-phasis.de
 *
 * Adapted from Haiku's Bluetooth server by Humdinger, 2022
 */


#include "DeskbarReplicant.h"
#include "App.h"
#include "Constants.h"

#include <AboutWindow.h>
#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <Catalog.h>
#include <Deskbar.h>
#include <IconUtils.h>
#include <MenuItem.h>
#include <Message.h>
#include <PopUpMenu.h>
#include <Resources.h>
#include <Roster.h>


extern "C" _EXPORT BView *instantiate_deskbar_item(float maxWidth, float maxHeight);
status_t our_image(image_info& image);

const char* kClassName = "DeskbarReplicant";


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Application"


//	#pragma mark -


DeskbarReplicant::DeskbarReplicant(BRect frame, int32 resizingMode)
	: BView(frame, "Clipdinger", resizingMode,
		B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_FRAME_EVENTS)
{
	_Init();
}


DeskbarReplicant::DeskbarReplicant(BMessage* archive)
	: BView(archive)
{
	_Init();
}


DeskbarReplicant::~DeskbarReplicant()
{
}


void
DeskbarReplicant::_Init()
{
	fIcon = NULL;

	image_info info;
	if (our_image(info) != B_OK)
		return;

	BFile file(info.name, B_READ_ONLY);
	if (file.InitCheck() < B_OK)
		return;

	BResources resources(&file);
	if (resources.InitCheck() < B_OK)
		return;

	size_t size;
	const void* data = resources.LoadResource(B_VECTOR_ICON_TYPE,
		"tray_icon", &size);
	if (data != NULL) {
		BBitmap* icon = new BBitmap(Bounds(), B_RGBA32);
		if (icon->InitCheck() == B_OK
				&& BIconUtils::GetVectorIcon((const uint8 *)data, size, icon) == B_OK)
			fIcon = icon;
		else
			delete icon;
	}
}


DeskbarReplicant *
DeskbarReplicant::Instantiate(BMessage* archive)
{
	if (!validate_instantiation(archive, kClassName))
		return NULL;

	return new DeskbarReplicant(archive);
}


status_t
DeskbarReplicant::Archive(BMessage* archive, bool deep) const
{
	status_t status = BView::Archive(archive, deep);
	if (status == B_OK)
		status = archive->AddString("add_on", kApplicationSignature);
	if (status == B_OK)
		status = archive->AddString("class", kClassName);

	return status;
}


void
DeskbarReplicant::AttachedToWindow()
{
	BView::AttachedToWindow();
	AdoptParentColors();

	if (ViewUIColor() == B_NO_COLOR)
		SetLowColor(ViewColor());
	else
		SetLowUIColor(ViewUIColor());
}


void
DeskbarReplicant::Draw(BRect updateRect)
{
	if (!fIcon) {
		/* At least display something... */
		rgb_color lowColor = LowColor();
		SetLowColor(0, 113, 187, 255);
		FillRoundRect(Bounds().InsetBySelf(3.f, 0.f), 5.f, 7.f, B_SOLID_LOW);
		SetLowColor(lowColor);
	} else {
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(fIcon);
		SetDrawingMode(B_OP_COPY);
	}
}


void
DeskbarReplicant::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case B_ABOUT_REQUESTED:
		{
			BMessenger messenger(kApplicationSignature);
			if (messenger.IsValid())
				messenger.SendMessage(msg);
			break;
		}
		case OPEN_CLIPDINGER:
		{
			BMessenger messenger(kApplicationSignature);
			if (messenger.IsValid())
				messenger.SendMessage(new BMessage(ACTIVATE));
			else
				be_roster->Launch(kApplicationSignature);	// In case we're not running (crashed?)
			break;
		}
		default:
			BView::MessageReceived(msg);
			break;
	}
}


void
DeskbarReplicant::MouseDown(BPoint where)
{
	BPoint point;
	uint32 buttons;
	GetMouse(&point, &buttons);

	if (buttons & B_SECONDARY_MOUSE_BUTTON) {

		BPopUpMenu* menu = new BPopUpMenu("", false, false);
		menu->SetFont(be_plain_font);

		menu->AddItem(new BMenuItem(B_TRANSLATE("Open Clipdinger"), new BMessage(OPEN_CLIPDINGER)));
		menu->AddItem(new BMenuItem(B_TRANSLATE("About Clipdinger"),
			new BMessage(B_ABOUT_REQUESTED)));

		menu->SetTargetForItems(this);
		ConvertToScreen(&point);
		menu->Go(point, true, true, BRect(where - BPoint(4, 4), 
			point + BPoint(4, 4)));

		delete menu;
	} else if (buttons & B_PRIMARY_MOUSE_BUTTON)
		BMessenger(this).SendMessage(OPEN_CLIPDINGER);

}


//	#pragma mark -


extern "C" _EXPORT BView *
instantiate_deskbar_item(float maxWidth, float maxHeight)
{
	return new DeskbarReplicant(BRect(0, 0, maxHeight - 1, maxHeight - 1),
		B_FOLLOW_NONE);
}


//	#pragma mark -


status_t
our_image(image_info& image)
{
	int32 cookie = 0;
	while (get_next_image_info(B_CURRENT_TEAM, &cookie, &image) == B_OK) {
		if ((char *)our_image >= (char *)image.text
			&& (char *)our_image <= (char *)image.text + image.text_size)
			return B_OK;
	}
	BAlert* alert = new BAlert("image", "Image NOT OK", "NOT");
	alert->Show();
	return B_ERROR;
}
