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




#ifndef GE_PIANOROLL_H
#define GE_PIANOROLL_H

#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include "../../core/recorder.h"
#include "ge_actionWidget.h"


class gPianoRollContainer : public Fl_Scroll {

private:
	class gdActionEditor *pParent;
	class gPianoRoll     *pianoRoll;

public:
	gPianoRollContainer(int x, int y, class gdActionEditor *parent);
	~gPianoRollContainer();
	void draw();
	void updateActions();
};





class gPianoRoll : public gActionWidget {

private:

	

	bool onItem(int rel_x, int rel_y);

	

	

	void drawSurface1();

	

	void drawSurface2();

	int  push_y;
	Fl_Offscreen surface1;  
	Fl_Offscreen surface2;  


public:
	gPianoRoll(int x, int y, int w, class gdActionEditor *pParent);

	void draw();
	int  handle(int e);
	void updateActions();

	enum { MAX_NOTES = 127, CELL_H = 15 };
};





class gPianoItem : public Fl_Box {

private:

	

	inline int getRelY() { return y() - parent()->y() - 3; };
	inline int getRelX() { return x() - parent()->x(); };

	

	inline int getNote(int rel_y) {
		return gPianoRoll::MAX_NOTES - (rel_y / gPianoRoll::CELL_H);
	};

	

	inline int getY(int note) {
		return (gPianoRoll::MAX_NOTES * gPianoRoll::CELL_H) - (note * gPianoRoll::CELL_H);
	};

	

	bool overlap();

	recorder::action *a;
	recorder::action *b;
	class gdActionEditor *pParent;

	bool selected;
	int  push_x;

	 

	int  note;
	int  frame_a;
	int  frame_b;

	

	int event_a;
	int event_b;

	

	bool changed;

	

	bool onLeftEdge;
	bool onRightEdge;

	

	int old_x, old_w;

public:

	

	gPianoItem(int x, int y, int rel_x, int rel_y, recorder::action *a, recorder::action *b, class gdActionEditor *pParent);

	void draw();
	int  handle(int e);
	void record();
	void remove();

	inline int getFrame_a() { return frame_a; }
	inline int getFrame_b() { return frame_b; }
	inline int getNote()    { return note;    }

};

#endif
