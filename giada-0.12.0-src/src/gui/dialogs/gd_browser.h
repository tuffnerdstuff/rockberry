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




#ifndef GD_BROWSER_H
#define GD_BROWSER_H


#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "../elems/ge_window.h"




class gdBrowser : public gWindow {

private:
	static void cb_down(Fl_Widget *v, void *p);
	static void cb_up  (Fl_Widget *v, void *p);
	static void cb_load_sample (Fl_Widget *v, void *p);
	static void cb_save_sample (Fl_Widget *v, void *p);
	static void cb_load_patch  (Fl_Widget *v, void *p);
	static void cb_save_patch  (Fl_Widget *v, void *p);
	static void cb_save_project(Fl_Widget *v, void *p);
	static void cb_close       (Fl_Widget *w, void *p);

	inline void __cb_down();
	inline void __cb_up();
	inline void __cb_load_sample();
	inline void __cb_save_sample();
	inline void __cb_save_project();
	inline void __cb_load_patch();
	inline void __cb_save_patch();
	inline void __cb_close();
	
	class gBrowser  *browser;
	class gClick    *ok;
	class gClick    *cancel;
	class gInput    *where;
	class gInput    *name;
 	class gClick    *updir;
 	class gProgress *status;

	class Channel *ch;

	

	
	int type;

	

	int stackType;

	char selectedFile[FILENAME_MAX];

public:
	gdBrowser(const char *title, const char *initPath, class Channel *ch, int type, int stackType=0);
	~gdBrowser();

	char* SelectedFile();
};

#endif
