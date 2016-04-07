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




#include <stdio.h>
#include "../utils/log.h"
#include "../glue/glue.h"
#include "kernelMidi.h"
#include "mixer.h"
#include "channel.h"
#include "sampleChannel.h"
#include "conf.h"
#include "midiMapConf.h"


extern bool        G_midiStatus;
extern Conf        G_Conf;
extern Mixer       G_Mixer;
extern MidiMapConf G_MidiMap;


using std::string;


namespace kernelMidi
{
int        api         = 0;      
RtMidiOut *midiOut     = NULL;
RtMidiIn  *midiIn      = NULL;
unsigned   numOutPorts = 0;
unsigned   numInPorts  = 0;

cb_midiLearn *cb_learn = NULL;
void         *cb_data  = NULL;


void __sendMidiLightningInitMsgs__()
{
	for(unsigned i=0; i<G_MidiMap.initCommands.size(); i++) {
		MidiMapConf::message_t msg = G_MidiMap.initCommands.at(i);
		if (msg.value != 0x0 && msg.channel != -1) {
			gLog("[KM] MIDI send (init) - Channel %x - Event 0x%X\n", msg.channel, msg.value);
			send(msg.value | MIDI_CHANS[msg.channel]);
		}
	}
}





void startMidiLearn(cb_midiLearn *cb, void *data)
{
	cb_learn = cb;
	cb_data  = data;
}





void stopMidiLearn()
{
	cb_learn = NULL;
	cb_data  = NULL;
}





void setApi(int _api)
{
	api = _api;
	gLog("[KM] using system 0x%x\n", api);
}





int openOutDevice(int port)
{
	try {
		midiOut = new RtMidiOut((RtMidi::Api) api, "Giada MIDI Output");
		G_midiStatus = true;
  }
  catch (RtMidiError &error) {
    gLog("[KM] MIDI out device error: %s\n", error.getMessage().c_str());
    G_midiStatus = false;
    return 0;
  }

	

	numOutPorts = midiOut->getPortCount();
  gLog("[KM] %d output MIDI ports found\n", numOutPorts);
  for (unsigned i=0; i<numOutPorts; i++)
		gLog("  %d) %s\n", i, getOutPortName(i).c_str());

	

	if (port != -1 && numOutPorts > 0) {
		try {
			midiOut->openPort(port, getOutPortName(port));
			gLog("[KM] MIDI out port %d open\n", port);

			

			__sendMidiLightningInitMsgs__();
			return 1;
		}
		catch (RtMidiError &error) {
			gLog("[KM] unable to open MIDI out port %d: %s\n", port, error.getMessage().c_str());
			G_midiStatus = false;
			return 0;
		}
	}
	else
		return 2;
}





int openInDevice(int port)
{
	try {
		midiIn = new RtMidiIn((RtMidi::Api) api, "Giada MIDI input");
		G_midiStatus = true;
  }
  catch (RtMidiError &error) {
    gLog("[KM] MIDI in device error: %s\n", error.getMessage().c_str());
    G_midiStatus = false;
    return 0;
  }

	

	numInPorts = midiIn->getPortCount();
  gLog("[KM] %d input MIDI ports found\n", numInPorts);
  for (unsigned i=0; i<numInPorts; i++)
		gLog("  %d) %s\n", i, getInPortName(i).c_str());

	

	if (port != -1 && numInPorts > 0) {
		try {
			midiIn->openPort(port, getInPortName(port));
			midiIn->ignoreTypes(true, false, true); 
			gLog("[KM] MIDI in port %d open\n", port);
			midiIn->setCallback(&callback);
			return 1;
		}
		catch (RtMidiError &error) {
			gLog("[KM] unable to open MIDI in port %d: %s\n", port, error.getMessage().c_str());
			G_midiStatus = false;
			return 0;
		}
	}
	else
		return 2;
}





bool hasAPI(int API)
{
	std::vector<RtMidi::Api> APIs;
	RtMidi::getCompiledApi(APIs);
	for (unsigned i=0; i<APIs.size(); i++)
		if (APIs.at(i) == API)
			return true;
	return false;
}





string getOutPortName(unsigned p)
{
	try { return midiOut->getPortName(p); }
	catch (RtMidiError &error) { return ""; }
}

string getInPortName(unsigned p)
{
	try { return midiIn->getPortName(p); }
	catch (RtMidiError &error) { return ""; }
}





void send(uint32_t data)
{
	if (!G_midiStatus)
		return;

  std::vector<unsigned char> msg(1, getB1(data));
  msg.push_back(getB2(data));
  msg.push_back(getB3(data));

	midiOut->sendMessage(&msg);
	gLog("[KM] send msg=0x%X (%X %X %X)\n", data, msg[0], msg[1], msg[2]);
}





void send(int b1, int b2, int b3)
{
	if (!G_midiStatus)
		return;

	std::vector<unsigned char> msg(1, b1);

	if (b2 != -1)
		msg.push_back(b2);
	if (b3 != -1)
		msg.push_back(b3);

	midiOut->sendMessage(&msg);
	
}





void callback(double t, std::vector<unsigned char> *msg, void *data)
{
	

	if (msg->size() < 3) {
		gLog("[KM] MIDI received - unknown signal - size=%d, value=0x", (int) msg->size());
		for (unsigned i=0; i<msg->size(); i++)
			gLog("%X", (int) msg->at(i));
		gLog("\n");
		return;
	}

	

	uint32_t input = getIValue(msg->at(0), msg->at(1), msg->at(2));
	uint32_t chan  = input & 0x0F000000;
	uint32_t value = input & 0x0000FF00;
	uint32_t pure  = 0x00;
	if (!G_Conf.noNoteOff)
		pure  = input & 0xFFFF0000;   
	else
		pure  = input & 0xFFFFFF00;   

	gLog("[KM] MIDI received - 0x%X (chan %d)", input, chan >> 24);

	

	if (cb_learn)	{
		gLog("\n");
		cb_learn(pure, cb_data);
	}
	else {

		

		if      (pure == G_Conf.midiInRewind) {
			gLog(" >>> rewind (global) (pure=0x%X)", pure);
			glue_rewindSeq();
		}
		else if (pure == G_Conf.midiInStartStop) {
			gLog(" >>> startStop (global) (pure=0x%X)", pure);
			glue_startStopSeq();
		}
		else if (pure == G_Conf.midiInActionRec) {
			gLog(" >>> actionRec (global) (pure=0x%X)", pure);
			glue_startStopActionRec();
		}
		else if (pure == G_Conf.midiInInputRec) {
			gLog(" >>> inputRec (global) (pure=0x%X)", pure);
			glue_startStopInputRec(false, false);   
		}
		else if (pure == G_Conf.midiInMetronome) {
			gLog(" >>> metronome (global) (pure=0x%X)", pure);
			glue_startStopMetronome(false);
		}
		else if (pure == G_Conf.midiInVolumeIn) {
			float vf = (value >> 8)/127.0f;
			gLog(" >>> input volume (global) (pure=0x%X, value=%d, float=%f)", pure, value >> 8, vf);
			glue_setInVol(vf, false);
		}
		else if (pure == G_Conf.midiInVolumeOut) {
			float vf = (value >> 8)/127.0f;
			gLog(" >>> output volume (global) (pure=0x%X, value=%d, float=%f)", pure, value >> 8, vf);
			glue_setOutVol(vf, false);
		}
		else if (pure == G_Conf.midiInBeatDouble) {
			gLog(" >>> sequencer x2 (global) (pure=0x%X)", pure);
			glue_beatsMultiply();
		}
		else if (pure == G_Conf.midiInBeatHalf) {
			gLog(" >>> sequencer /2 (global) (pure=0x%X)", pure);
			glue_beatsDivide();
		}

		

		for (unsigned i=0; i<G_Mixer.channels.size(); i++) {

			Channel *ch = (Channel*) G_Mixer.channels.at(i);

			if (!ch->midiIn) continue;

			if      (pure == ch->midiInKeyPress) {
				gLog(" >>> keyPress, ch=%d (pure=0x%X)", ch->index, pure);
				glue_keyPress(ch, false, false);
			}
			else if (pure == ch->midiInKeyRel) {
				gLog(" >>> keyRel ch=%d (pure=0x%X)", ch->index, pure);
				glue_keyRelease(ch, false, false);
			}
			else if (pure == ch->midiInMute) {
				gLog(" >>> mute ch=%d (pure=0x%X)", ch->index, pure);
				glue_setMute(ch, false);
			}
			else if (pure == ch->midiInSolo) {
				gLog(" >>> solo ch=%d (pure=0x%X)", ch->index, pure);
				ch->solo ? glue_setSoloOn(ch, false) : glue_setSoloOff(ch, false);
			}
			else if (pure == ch->midiInVolume) {
				float vf = (value >> 8)/127.0f;
				gLog(" >>> volume ch=%d (pure=0x%X, value=%d, float=%f)", ch->index, pure, value >> 8, vf);
				glue_setChanVol(ch, vf, false);
			}
			else if (pure == ((SampleChannel*)ch)->midiInPitch) {
				float vf = (value >> 8)/(127/4.0f); 
				gLog(" >>> pitch ch=%d (pure=0x%X, value=%d, float=%f)", ch->index, pure, value >> 8, vf);
				glue_setPitch(NULL, (SampleChannel*)ch, vf, false);
			}
			else if (pure == ((SampleChannel*)ch)->midiInReadActions) {
				gLog(" >>> start/stop read actions ch=%d (pure=0x%X)", ch->index, pure);
				glue_startStopReadingRecs((SampleChannel*)ch, false);
			}
		}
		gLog("\n");
	}
}





std::string getRtMidiVersion()
{
	return midiOut->getVersion();
}





}  
