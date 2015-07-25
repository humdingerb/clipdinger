/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define OUTPUT_PORT_NAME	"Clipdinger output port"

static const char *kApplicationSignature = "application/x-vnd.Clipdinger";
static const char kSettingsFolder[] = "Clipdinger";
static const char kSettingsFile[] = "Clipdinger_settings";
static const char kHistoryFile[] = "Clipdinger_history";
static const char kFavoriteFile[] = "Clipdinger_favorites";

static const int32 kDefaultLimit = 50;
static const int32 kDefaultAutoPaste = 1;
static const int32 kDefaultFade = 0;
static const int32 kDefaultFadeDelay = 1;
static const int32 kDefaultFadeStep = 4;
static const int32 kIconSize = 16;
static const int32 kMaxTitleChars = 200;
static const int32 kMinuteUnits = 5; // minutes per unit

#define DELETE				'dele'
#define DELETE_FAV			'delf'
#define ADD_FAV				'addf'
#define ESCAPE				'esca'
#define POPCLOSED			'pmcl'

#define CLEAR_HISTORY		'clhi'
#define HELP				'help'
#define	UP					'favu'
#define DOWN				'favd'
#define INSERT_HISTORY		'ihis'
#define INSERT_FAVORITE		'ifav'
#define ADJUSTCOLORS		'acol'
#define SETTINGS			'sett'

#define	AUTOPASTE			'auto'
#define FADE				'fade'
#define DELAY				'dely'
#define STEP				'step'
#define CANCEL				'cncl'
#define OK					'okay'
#define UPDATE_SETTINGS		'uset'

#endif //CONSTANTS_H
