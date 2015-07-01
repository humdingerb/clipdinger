/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 *
 * Minimal changes by Humdinger, humdingerb@gmail.com 
 */

#ifndef _INPUT_FILTER_H_
#define _INPUT_FILTER_H_

#include <InputServerFilter.h>

extern "C" _EXPORT BInputServerFilter* instantiate_input_filter();


class InputFilter : public BInputServerFilter {
public:
					InputFilter();
	
	filter_result	Filter(BMessage* message, BList* outList);
	status_t		InitCheck();
};

#endif _INPUT_FILTER_H_
