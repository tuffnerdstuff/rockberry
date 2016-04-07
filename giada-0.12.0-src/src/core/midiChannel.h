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




#ifndef MIDI_CHANNEL_H
#define MIDI_CHANNEL_H


#ifdef WITH_VST

#include "../deps/juce/juce_audio_basics/juce_audio_basics.h"
#include "../deps/juce/juce_audio_processors/juce_audio_processors.h"
#include "../deps/juce/juce_core/juce_core.h"
#include "../deps/juce/juce_data_structures/juce_data_structures.h"
#include "../deps/juce/juce_events/juce_events.h"
#include "../deps/juce/juce_graphics/juce_graphics.h"
#include "../deps/juce/juce_gui_basics/juce_gui_basics.h"
#include "../deps/juce/juce_gui_extra/juce_gui_extra.h"
#endif

#include "channel.h"


class MidiChannel : public Channel
{
public:

	MidiChannel(int bufferSize);
	~MidiChannel();

  bool    midiOut;           
  uint8_t midiOutChan;       

	void copy       (const Channel *src);
	void process    (float *buffer);
	void start      (int frame, bool doQuantize);
	void kill       (int frame);
	void empty      ();
	void stopBySeq  ();
	void stop       ();
	void rewind     ();
	void setMute    (bool internal);
	void unsetMute  (bool internal);
	int  readPatch_DEPR_  (const char *file, int i);
	int  readPatch  (const string &basePath, int i);
	int  writePatch (int i, bool isProject);
	void quantize   (int index, int localFrame, int globalFrame);
	void onZero     (int frame);
	void onBar      (int frame);
	void parseAction(recorder::action *a, int localFrame, int globalFrame);

	

	

	void sendMidi(recorder::action *a, int localFrame);
	void sendMidi(uint32_t data);

#ifdef WITH_VST

	

	VstEvents *getVstEvents();

	

	juce::MidiBuffer &getPluginMidiEvents();

	

	void freeVstMidiEvents(bool init=false);

	

	void addVstMidiEvent(uint32_t msg, int localFrame);

#endif

	

#ifdef WITH_VST

	

	juce::MidiBuffer midiBuffer;

#endif

};


#endif
