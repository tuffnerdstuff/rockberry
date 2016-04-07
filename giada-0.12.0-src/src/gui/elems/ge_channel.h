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




#ifndef GE_CHANNEL_H
#define GE_CHANNEL_H


#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Button.H>
#include "ge_mixed.h"


class gChannel : public Fl_Group
{
protected:

	

#ifdef WITH_VST
	static const int BREAK_READ_ACTIONS = 212;
	static const int BREAK_MODE_BOX     = 188;
	static const int BREAK_FX           = 164;
	static const int BREAK_DELTA        = 120;
#else
	static const int BREAK_READ_ACTIONS = 188;
	static const int BREAK_MODE_BOX     = 164;
	static const int BREAK_FX           = 140;
	static const int BREAK_DELTA        = 96;
#endif
	static const int BREAK_UNIT         = 24;

	

	void blink();

	

	void setColorsByStatus(int playStatus, int recStatus);

	

	int handleKey(int e, int key);

public:

	gChannel(int x, int y, int w, int h, int type);

	

	virtual void reset() = 0;

	

	virtual void update() = 0;

	

	virtual void refresh() = 0;

	

	virtual int keyPress(int event) = 0;

	

	int getColumnIndex();

	class gButton        *button;
	class gStatus        *status;
	class gChannelButton *mainButton;
	class gDial          *vol;
	class gClick 	       *mute;
	class gClick 	       *solo;
#ifdef WITH_VST
	class gFxButton      *fx;
#endif

	int type;
};


#endif
