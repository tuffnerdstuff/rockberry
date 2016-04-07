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




#ifndef GE_KEYBOARD_H
#define GE_KEYBOARD_H


#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Button.H>
#include <vector>
#include "../elems/ge_column.h"
#include "../../core/const.h"
#include "../../utils/utils.h"


using std::vector;


class gKeyboard : public Fl_Scroll
{
private:

	

	void refreshColIndexes();

	static void cb_addColumn  (Fl_Widget *v, void *p);
	inline void __cb_addColumn(int width=DEFAULT_COLUMN_WIDTH);

	bool bckspcPressed;
	bool endPressed;
	bool spacePressed;
	bool enterPressed;

	

	static int indexColumn;

	class gClick *addColumnBtn;

	

	vector<gColumn*> columns;

public:

	gKeyboard(int X, int Y, int W, int H);

	int handle(int e);

	

	void init();

	

	gChannel *addChannel(int column, class Channel *ch, bool build=false);

	

	void addColumn(int width=380);

	

	void deleteChannel(gChannel *gch);

	

	void freeChannel(gChannel *gch);

	

	void updateChannel(gChannel *gch);

	

	void organizeColumns();

	

	void refreshColumns();

	

	gColumn *getColumnByIndex(int index);

	

	inline gColumn *getColumn(int i) { return columns.at(i); }

	

	void clear();

	

	void setChannelWithActions(class gSampleChannel *gch);

	

	void printChannelMessage(int res);

	

	inline unsigned getTotalColumns() { return columns.size(); }

	

	inline int getColumnWidth(int i) { return getColumnByIndex(i)->w(); }
};


#endif
