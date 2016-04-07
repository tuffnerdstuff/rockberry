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




#include "../../core/graphics.h"
#include "../../core/mixer.h"
#include "../elems/ge_mixed.h"
#include "../elems/ge_waveform.h"
#include "ge_waveTools.h"


gWaveTools::gWaveTools(int x, int y, int w, int h, SampleChannel *ch, const char *l)
	: Fl_Scroll(x, y, w, h, l)
{
	type(Fl_Scroll::HORIZONTAL_ALWAYS);
	hscrollbar.color(COLOR_BG_0);
	hscrollbar.selection_color(COLOR_BG_1);
	hscrollbar.labelcolor(COLOR_BD_1);
	hscrollbar.slider(G_BOX);

	waveform = new gWaveform(x, y, w, h-24, ch);


	
}






void gWaveTools::updateWaveform() 
{
	waveform->alloc(w());
	waveform->redraw();
}





void gWaveTools::resize(int x, int y, int w, int h) 
{
	if (this->w() == w || (this->w() != w && this->h() != h)) {   
		Fl_Widget::resize(x, y, w, h);
		waveform->resize(x, y, waveform->w(), h-24);
		updateWaveform();
	}
	else {                                                        
		Fl_Widget::resize(x, y, w, h);
	}

	if (this->w() > waveform->w())
		waveform->stretchToWindow();

	int offset = waveform->x() + waveform->w() - this->w() - this->x();
	if (offset < 0)
		waveform->position(waveform->x()-offset, this->y());
}





int gWaveTools::handle(int e) 
{
	int ret = Fl_Group::handle(e);
	switch (e) {
		case FL_MOUSEWHEEL: {
			waveform->setZoom(Fl::event_dy());
			redraw();
			ret = 1;
			break;
		}
	}
	return ret;
}

