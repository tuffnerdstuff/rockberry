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



#ifndef GE_ACTIONCHANNEL_H
#define GE_ACTIONCHANNEL_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include "../../utils/gui_utils.h"
#include "../../core/mixer.h"
#include "../../core/recorder.h"
#include "ge_actionWidget.h"


class gAction : public Fl_Box {

private:

	bool                  selected;
	unsigned              index;
  class gdActionEditor *parent;   
	class SampleChannel  *ch;
  char                  type;     

public:
	gAction(int x, int y, int h, int frame_a, unsigned index,
				  gdActionEditor *parent, class SampleChannel *ch, bool record,
			    char type);
	void draw();
	int  handle(int e);
	void addAction();
	void delAction();

	

	void moveAction(int frame_a=-1);

	

	int  absx();

	

	int xToFrame_a();
	int xToFrame_b();

	int frame_a;  
	int frame_b;  

	bool onRightEdge;
	bool onLeftEdge;

	static const int MIN_WIDTH;
};





class gActionChannel : public gActionWidget {

private:

	class SampleChannel *ch;

	

	gAction *getSelectedAction();

	

	gAction *selected;

	

	int actionOriginalX;
	int actionOriginalW;

	

	int actionPickPoint;


	

	bool actionCollides(int frame);

public:
	gActionChannel(int x, int y, gdActionEditor *pParent, class SampleChannel *ch);
	void draw();
	int  handle(int e);
	void updateActions();
};


#endif
