/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2016 Giovanni A. Zuliani | Monocasual
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */



#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <dirent.h>
#include <string>
#include <FL/x.H>
#include <FL/Fl.H>
#ifdef __APPLE__
	#include <libgen.h>	
#endif



#if defined(_WIN32)
	#include "../ext/resource.h"
#elif defined(__linux__)
	#include <X11/xpm.h>
#endif


using std::string;




void gu_refresh();



int gu_getBlinker();



void gu_updateControls();



void gu_update_win_label(const char *c);

void gu_setFavicon(Fl_Window *w);

void gu_openSubWindow(class gWindow *parent, gWindow *child, int id);



void gu_refreshActionEditor();




void gu_closeAllSubwindows();




gWindow *gu_getSubwindow(class gWindow *parent, int id);



string gu_removeFltkChars(const string &s);

#endif
