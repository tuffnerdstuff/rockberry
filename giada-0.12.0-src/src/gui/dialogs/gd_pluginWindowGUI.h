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





#ifdef WITH_VST


#ifndef __GD_PLUGIN_WINDOW_GUI_H__
#define __GD_PLUGIN_WINDOW_GUI_H__


#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "../elems/ge_window.h"
#if defined(__APPLE__)
	#include <Carbon/Carbon.h>
#endif


class gdPluginWindowGUI : public gWindow
{
private:

	class Plugin *pPlugin;

	static void cb_close    (Fl_Widget *v, void *p);
	static void cb_refresh  (void *data);
	inline void __cb_close  ();
	inline void __cb_refresh();

public:

	gdPluginWindowGUI(Plugin *pPlugin);
	~gdPluginWindowGUI();
};




#if 0
#if defined(__APPLE__)

class gdPluginWindowGUImac : public gWindow
{
private:

	static pascal OSStatus windowHandler(EventHandlerCallRef ehc, EventRef e, void *data);
	inline pascal OSStatus __wh(EventHandlerCallRef ehc, EventRef e);

	class Plugin *pPlugin;
	WindowRef carbonWindow;
	bool open;

public:

	gdPluginWindowGUImac(Plugin *pPlugin);
	~gdPluginWindowGUImac();
};

#endif
#endif

#endif 

#endif 
