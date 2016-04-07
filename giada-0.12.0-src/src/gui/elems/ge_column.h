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




#ifndef GE_COLUMN_H
#define GE_COLUMN_H


#include <FL/Fl.H>
#include <FL/Fl_Group.H>


class gColumn : public Fl_Group
{
private:

	static void cb_addChannel  (Fl_Widget *v, void *p);
	inline void __cb_addChannel();

	int openTypeMenu();

	class gClick      *addChannelBtn;
	class gResizerBar *resizer;
	class gKeyboard   *parent;

	int index;

public:

	gColumn(int x, int y, int w, int h, int index, class gKeyboard *parent);
	~gColumn();

	

	class gChannel *addChannel(class Channel *ch);

	

	int handle(int e);

  

  void resize(int x, int y, int w, int h);

	

	void deleteChannel(gChannel *gch);

	

	void refreshChannels();

  

  Channel *getChannel(int i);

	

	void clear(bool full=false);

	void draw();

	inline int  getIndex()      { return index; }
	inline void setIndex(int i) { index = i; }
	inline bool isEmpty()       { return children() == 1; }
  inline int  countChannels() { return children(); }
};


#endif
