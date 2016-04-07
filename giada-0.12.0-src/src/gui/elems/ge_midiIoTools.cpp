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




#include "ge_midiIoTools.h"
#include "ge_mixed.h"


gLearner::gLearner(int X, int Y, int W, const char *l, kernelMidi::cb_midiLearn *cb, uint32_t *param)
	: Fl_Group(X, Y, W, 20),
		callback(cb),
		param   (param)
{
	begin();
	text   = new gBox(x(), y(), 156, 20, l);
	value  = new gClick(text->x()+text->w()+4, y(), 80, 20, "(not set)");
	button = new gButton(value->x()+value->w()+4, y(), 40, 20, "learn");
	end();

	text->box(G_BOX);
	text->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

	value->box(G_BOX);
	value->callback(cb_value, (void*)this);
	value->when(FL_WHEN_RELEASE);
	updateValue();

	button->type(FL_TOGGLE_BUTTON);
	button->callback(cb_button, (void*)this);
}





void gLearner::updateValue() {
	char buf[16];
	if (*param != 0x0)
		snprintf(buf, 9, "0x%X", *param);
	else
		snprintf(buf, 16, "(not set)");
	value->copy_label(buf);
	button->value(0);
}





void gLearner::cb_button(Fl_Widget *v, void *p) { ((gLearner*)p)->__cb_button(); }
void gLearner::cb_value(Fl_Widget *v, void *p) { ((gLearner*)p)->__cb_value(); }





void gLearner::__cb_value() {
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		*param = 0x0;
		updateValue();
	}
	
}







void gLearner::__cb_button() {
	if (button->value() == 1) {
		cbData *data  = (cbData*) malloc(sizeof(cbData));
		data->window  = (gdMidiInput*) parent();  
		data->learner = this;
		kernelMidi::startMidiLearn(callback, (void*)data);
	}
	else
		kernelMidi::stopMidiLearn();
}
