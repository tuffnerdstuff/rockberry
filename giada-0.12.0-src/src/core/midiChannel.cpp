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




#include "../utils/log.h"
#include "midiChannel.h"
#include "channel.h"
#include "pluginHost.h"
#include "patch_DEPR_.h"
#include "patch.h"
#include "conf.h"
#include "kernelMidi.h"


extern Patch_DEPR_ G_Patch_DEPR_;
extern Patch       G_Patch;
extern Mixer       G_Mixer;
extern Conf        G_Conf;
#ifdef WITH_VST
extern PluginHost  G_PluginHost;
#endif


MidiChannel::MidiChannel(int bufferSize)
	: Channel    (CHANNEL_MIDI, STATUS_OFF, bufferSize),
	  midiOut    (false),
	  midiOutChan(MIDI_CHANS[0])
{
#ifdef WITH_VST 
	freeVstMidiEvents(true);
#endif
}





MidiChannel::~MidiChannel() {}





void MidiChannel::copy(const Channel *_src)
{
	Channel::copy(_src);
	MidiChannel *src = (MidiChannel *) _src;
	midiOut     = src->midiOut;
	midiOutChan = src->midiOutChan;
}





#ifdef WITH_VST

void MidiChannel::freeVstMidiEvents(bool init)
{
	midiBuffer.clear();
}

#endif





#ifdef WITH_VST

void MidiChannel::addVstMidiEvent(uint32_t msg, int localFrame)
{
	juce::MidiMessage message = juce::MidiMessage(
		kernelMidi::getB1(msg),
		kernelMidi::getB2(msg),
		kernelMidi::getB3(msg));

	midiBuffer.addEvent(message, localFrame);
}

#endif





void MidiChannel::onBar(int frame) {}





void MidiChannel::stop() {}





void MidiChannel::empty() {}





void MidiChannel::quantize(int index, int localFrame, int globalFrame) {}




#ifdef WITH_VST

VstEvents *MidiChannel::getVstEvents()
{
#if 0
	return (VstEvents *) &events;
#endif
	return NULL;
}

juce::MidiBuffer &MidiChannel::getPluginMidiEvents()
{
	return midiBuffer;
}

#endif





void MidiChannel::parseAction(recorder::action *a, int localFrame, int globalFrame)
{
	if (a->type == ACTION_MIDI)
		sendMidi(a, localFrame/2);
}





void MidiChannel::onZero(int frame)
{
	if (status == STATUS_ENDING) {
		status = STATUS_OFF;
		sendMidiLplay();
	}
	else
	if (status == STATUS_WAIT) {
		status = STATUS_PLAY;
		sendMidiLplay();
	}
}





void MidiChannel::setMute(bool internal)
{
	mute = true;  	
	if (midiOut)
		kernelMidi::send(MIDI_ALL_NOTES_OFF);
#ifdef WITH_VST
		addVstMidiEvent(MIDI_ALL_NOTES_OFF, 0);
#endif
	sendMidiLmute();
}





void MidiChannel::unsetMute(bool internal)
{
	mute = false;  	
	sendMidiLmute();
}





void MidiChannel::process(float *buffer)
{
#ifdef WITH_VST
	G_PluginHost.processStack(vChan, PluginHost::CHANNEL, this);
	freeVstMidiEvents();
#endif

	for (int j=0; j<bufferSize; j+=2) {
		buffer[j]   += vChan[j]   * volume; 
		buffer[j+1] += vChan[j+1] * volume; 
	}
}





void MidiChannel::start(int frame, bool doQuantize)
{
	switch (status) {
		case STATUS_PLAY:
			status = STATUS_ENDING;
			sendMidiLplay();
			break;
		case STATUS_ENDING:
		case STATUS_WAIT:
			status = STATUS_OFF;
			sendMidiLplay();
			break;
		case STATUS_OFF:
			status = STATUS_WAIT;
			sendMidiLplay();
			break;
	}
}





void MidiChannel::stopBySeq()
{
	kill(0);
}





void MidiChannel::kill(int frame)
{
	if (status & (STATUS_PLAY | STATUS_ENDING)) {
		if (midiOut)
			kernelMidi::send(MIDI_ALL_NOTES_OFF);
#ifdef WITH_VST
		addVstMidiEvent(MIDI_ALL_NOTES_OFF, 0);
#endif
	}
	status = STATUS_OFF;
	sendMidiLplay();
}





int MidiChannel::readPatch_DEPR_(const char *f, int i)
{
	volume      = G_Patch_DEPR_.getVol(i);
	index       = G_Patch_DEPR_.getIndex(i);
	mute        = G_Patch_DEPR_.getMute(i);
	mute_s      = G_Patch_DEPR_.getMute_s(i);
	solo        = G_Patch_DEPR_.getSolo(i);
	panLeft     = G_Patch_DEPR_.getPanLeft(i);
	panRight    = G_Patch_DEPR_.getPanRight(i);

	midiOut     = G_Patch_DEPR_.getMidiValue(i, "Out");
	midiOutChan = G_Patch_DEPR_.getMidiValue(i, "OutChan");

	readPatchMidiIn_DEPR_(i);
	readPatchMidiOut_DEPR_(i);

	return SAMPLE_LOADED_OK;  
}





int MidiChannel::readPatch(const string &basePath, int i)
{
	Channel::readPatch("", i);

	Patch::channel_t *pch = &G_Patch.channels.at(i);

	midiOut     = pch->midiOut;
	midiOutChan = pch->midiOutChan;

	return SAMPLE_LOADED_OK;  
}





void MidiChannel::sendMidi(recorder::action *a, int localFrame)
{
	if (status & (STATUS_PLAY | STATUS_ENDING) && !mute) {
		if (midiOut)
			kernelMidi::send(a->iValue | MIDI_CHANS[midiOutChan]);

#ifdef WITH_VST
		addVstMidiEvent(a->iValue, localFrame);
#endif
	}
}


void MidiChannel::sendMidi(uint32_t data)
{
	if (status & (STATUS_PLAY | STATUS_ENDING) && !mute) {
		if (midiOut)
			kernelMidi::send(data | MIDI_CHANS[midiOutChan]);
#ifdef WITH_VST
		addVstMidiEvent(data, 0);
#endif
	}
}





void MidiChannel::rewind()
{
	if (midiOut)
		kernelMidi::send(MIDI_ALL_NOTES_OFF);
#ifdef WITH_VST
		addVstMidiEvent(MIDI_ALL_NOTES_OFF, 0);
#endif
}





int MidiChannel::writePatch(int i, bool isProject)
{
	int pchIndex = Channel::writePatch(i, isProject);
	Patch::channel_t *pch = &G_Patch.channels.at(pchIndex);

	pch->midiOut     = midiOut;
	pch->midiOutChan = midiOutChan;

	return 0;
}
