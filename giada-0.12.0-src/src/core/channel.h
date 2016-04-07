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




#ifndef CHANNEL_H
#define CHANNEL_H


#include <vector>
#include "../utils/utils.h"
#include "midiMapConf.h"
#include "const.h"
#include "recorder.h"


using std::vector;
using std::string;


class Channel
{
protected:

	

	int bufferSize;

	

	void sendMidiLmessage(uint32_t learn, const MidiMapConf::message_t &msg);

public:

	Channel(int type, int status, int bufferSize);

	virtual ~Channel();

	

	virtual void copy(const Channel *src) = 0;

	

	virtual int writePatch(int i, bool isProject);

	

	virtual int readPatch_DEPR_(const char *file, int i) = 0;
	virtual int readPatch(const string &basePath, int i);

	

	virtual void process(float *buffer) = 0;

	

	virtual void start(int frame, bool doQuantize) = 0;

	

	virtual void stop() = 0;

	

	virtual void kill(int frame) = 0;

	

	virtual void setMute  (bool internal) = 0;
	virtual void unsetMute(bool internal) = 0;

	

	virtual void empty() = 0;

	

	virtual void stopBySeq() = 0;

	

	virtual void quantize(int index, int localFrame, int globalFrame) = 0;

	

	virtual void onZero(int frame) = 0;

	

	virtual void onBar(int frame) = 0;

	

	virtual void parseAction(recorder::action *a, int localFrame, int globalFrame) = 0;

	

	virtual void rewind() = 0;

	

	int     index;                
	int     type;                 
	int     status;	              
	int     key;                  
	float   volume;               
	float   volume_i;             
	float   volume_d;             
	float   panLeft;
	float   panRight;
	bool    mute_i;               
	bool 	  mute_s;               
	bool    mute;                 
	bool    solo;
  bool    hasActions;           
	int 	  recStatus;            
	float  *vChan;	              
  class   gChannel *guiChannel; 

	

  bool     midiIn;              
  uint32_t midiInKeyPress;
  uint32_t midiInKeyRel;
  uint32_t midiInKill;
  uint32_t midiInVolume;
  uint32_t midiInMute;
  uint32_t midiInSolo;

	

	bool     midiOutL;
  uint32_t midiOutLplaying;
  uint32_t midiOutLmute;
  uint32_t midiOutLsolo;

#ifdef WITH_VST
  vector <class Plugin_DEPR_ *> plugins_DEPR_;
  vector <class Plugin *> plugins;
#endif


	

	

	bool isPlaying();

	

	void readPatchMidiIn_DEPR_(int i);
	void readPatchMidiOut_DEPR_(int i);

	

	void sendMidiLmute();
	void sendMidiLsolo();
	void sendMidiLplay();
};


#endif
