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




#ifndef GE_MUTECHANNEL_H
#define GE_MUTECHANNEL_H


#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include "../../utils/utils.h"
#include "ge_actionWidget.h"


using std::vector;


class gMuteChannel : public gActionWidget
{
private:

	

	struct point {
		int  frame;
		char type;
		int  x;
	};

	

	vector<point> points;

	

	int draggedPoint;

	

	int selectedPoint;

	

	int previousXPoint;

	
	void extractPoints();

	
	int getSelectedPoint();

	

	bool pointCollides(int frame);

public:

	gMuteChannel(int x, int y, class gdActionEditor *pParent);
	void draw();
	int  handle(int e);

	

	void updateActions();
};

#endif
