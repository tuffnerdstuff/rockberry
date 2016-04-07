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




#ifndef GE_WAVEFORM_H
#define GE_WAVEFORM_H


#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <math.h>
#include "../../utils/utils.h"


using std::vector;


#define  FLAG_WIDTH  14
#define  FLAG_HEIGHT 12
#define  BORDER      8				


class gWaveform : public Fl_Widget {

private:

	

	struct data {
		int *sup;
		int *inf;
		int  size;
	} data;

	

	struct grid {
		bool snap;
		int  level;
		vector<int> points;
	} grid;

	

	class SampleChannel *chan;

	

	bool menuOpen;

	

	bool mouseOnStart();
	bool mouseOnEnd();

	

	bool mouseOnSelectionA();
	bool mouseOnSelectionB();

	

	int absolutePoint(int p);

	

	int relativePoint(int p);

	

	void straightSel();

	

	void freeData();

	

	bool smaller();

  

  int applySnap(int pos);

public:

	gWaveform(int x, int y, int w, int h, class SampleChannel *ch, const char *l=0);
	~gWaveform();
	void draw();
	int  handle(int e);

	

	int alloc(int datasize=0);

	

	void recalcPoints();

	

	void openEditMenu();

	

	inline float displayRatio() { return 1.0f / (data.size / (float) w()); };

	

	void setZoom(int type);

	

	void stretchToWindow();

	

	void setGridLevel(int l);

  inline void setSnap(bool v) { grid.snap = v; }
  inline bool getSnap()       { return grid.snap; }

	inline int getSize() { return data.size; }

	int  chanStart;
	bool chanStartLit;
	int  chanEnd;
	bool chanEndLit;
	bool pushed;
	bool dragged;
	bool resized;

	float ratio;

  
	int  mouseX;					 
	int  mouseY;

	
	
	int selectionA;
	int selectionB;
	int selectionA_abs;
	int selectionB_abs;
};


#endif
