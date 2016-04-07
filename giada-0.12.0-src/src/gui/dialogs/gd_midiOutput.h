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




#ifndef GD_MIDI_OUTPUT_H
#define GD_MIDI_OUTPUT_H


#include <FL/Fl.H>
#include <stdint.h>
#include "../elems/ge_window.h"






class gdMidiOutput : public gWindow
{
protected:

	class gClick *close;
	class gCheck *enableLightning;

	void stopMidiLearn(class gLearner *l);

	

	static void cb_learn  (uint32_t msg, void *data);
	inline void __cb_learn(uint32_t *param, uint32_t msg, class gLearner *l);

	

	static void cb_close  (Fl_Widget *w, void *p);
	inline void __cb_close();

	

	static void cb_enableLightning  (Fl_Widget *w, void *p);
	inline void __cb_enableLightning();

	

	void setTitle(int chanNum);

public:

	gdMidiOutput(int w, int h);
};





class gdMidiOutputMidiCh : public gdMidiOutput
{
private:

	static void cb_enableChanList  (Fl_Widget *w, void *p);
	inline void __cb_enableChanList();

	

	static void cb_close  (Fl_Widget *w, void *p);
	inline void __cb_close();

	class gCheck  *enableOut;
	class gChoice *chanListOut;

	class MidiChannel *ch;

public:

	gdMidiOutputMidiCh(class MidiChannel *ch);
};





class gdMidiOutputSampleCh : public gdMidiOutput
{
private:

	class SampleChannel *ch;

	

	static void cb_close  (Fl_Widget *w, void *p);
	inline void __cb_close();

public:

	gdMidiOutputSampleCh(class SampleChannel *ch);
};

#endif
