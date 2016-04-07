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




#ifndef GE_MODEBOX_H
#define GE_MODEBOX_H


#include <FL/Fl_Menu_Button.H>


class gModeBox : public Fl_Menu_Button
{
private:

	static void cb_changeMode  (Fl_Widget *v, void *p);
	inline void __cb_changeMode(int mode);

	class SampleChannel *ch;

public:
	
  gModeBox(int x, int y, int w, int h, class SampleChannel *ch, const char *l=0);
	void draw();
};


#endif
