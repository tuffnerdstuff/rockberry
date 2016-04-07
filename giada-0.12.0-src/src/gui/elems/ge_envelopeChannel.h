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



#ifndef __GE_ENVELOPECHANNEL_H__
#define __GE_ENVELOPECHANNEL_H__


#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include "../../utils/utils.h"
#include "ge_actionWidget.h"


using std::vector;


class gEnvelopeChannel : public gActionWidget
{
	const char *l;      
	int         type;   
	int         range;

	

	struct point {
		int   frame;
		int   iValue;
		float fValue;
		int   x;
		int   y;
	};

	

	vector<point> points;

	

	int selectedPoint;

	

	int draggedPoint;

	

	int previousXPoint;

	void draw();

	int handle(int e);

	int getSelectedPoint();

	void sortPoints();

	

	int verticalPoint(const point &p);

public:
	gEnvelopeChannel(int x, int y, gdActionEditor *pParent, int type, int range, const char *l);
	~gEnvelopeChannel();

	

	void addPoint(int frame, int iValue=0, float fValue=0.0f, int x=-1, int y=-1);

	void updateActions();

	

	void fill();

	inline void clearPoints() { points.clear(); }
};

#endif
