/*
 * Copyright 2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define INPUT_PORT_NAME		"Clipdinger input port"
#define OUTPUT_PORT_NAME	"Clipdinger output port"

static const char *kApplicationSignature = "application/x-vnd.Clipdinger";
static const char kSettingsFolder[] = "Clipdinger";
static const char kSettingsFile[] = "Clipdinger_settings";
static const char kHistoryFile[] = "Clipdinger_history";

static const bool kDefaultFade = 0;
static const int32 kDefaultFadeDelay = 1;
static const float kDefaultFadeStep = 0.04;
static const int32 kDefaultLimit = 50;
static const int32 kIconSize = 16;
static const int32 kMaxTitleChars = 200;

#define PORTQUEUE			'port'

#define DELETE				'dele'
#define ESCAPE				'esca'
#define POPCLOSED			'pmcl'

#define CLEAR_HISTORY		'clhi'
#define INSERT_HISTORY		'ihis'
//#define INSERT_FAVORITE	'ifav'
#define ADJUSTCOLORS		'acol'
#define SETTINGS			'sett'

#define FADE				'fade'
#define SPEED				'sped'
#define STEP				'step'
#define CANCEL				'cncl'
#define OK					'okay'

#endif //CONSTANTS_H
