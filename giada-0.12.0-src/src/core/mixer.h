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




#ifndef MIXER_H
#define MIXER_H

#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include "const.h"
#include "kernelAudio.h"
#include "../utils/utils.h"


using std::vector;


class Mixer
{
public:

	Mixer();
	~Mixer();

	void init();
	int  close();

	

	class Channel *addChannel(int type);

	

	int deleteChannel(class Channel *ch);

	

	static int masterPlay(
		void *out_buf, void *in_buf, unsigned n_frames,
		double streamTime, RtAudioStreamStatus status, void *userData
	);
	int __masterPlay(void *out_buf, void *in_buf, unsigned n_frames);

	

	void updateFrameBars();

	

	bool isSilent();

	

	void rewind();

	

	void updateQuanto();

	

	bool hasLogicalSamples();

	

	bool hasEditedSamples();

	

	bool mergeVirtualInput();

	

	Channel *getChannelByIndex(int i);

	

	inline Channel* getLastChannel() { return channels.back(); }


	


	enum {    
		DO_STOP   = 0x01,
		DO_MUTE   = 0x02,
		DO_MUTE_I = 0x04
	};

	enum {    
		FADEOUT = 0x01,
		XFADE   = 0x02
	};

	vector<class Channel*> channels;

	bool   running;
	bool   ready;
	float *vChanInput;        
	float *vChanInToOut;      
	int    frameSize;
	float  outVol;
	float  inVol;
	float  peakOut;
	float  peakIn;
	int    quanto;
	char   quantize;
	bool	 metronome;
	float  bpm;
	int    bars;
	int    beats;
	int    waitRec;      

	bool docross;			   
	bool rewindWait;	   

	int framesPerBar;      
	int framesPerBeat;     
	int framesInSequencer; 
	int totalFrames;       
	int actualFrame;
	int actualBeat;

#define TICKSIZE 38
	static float tock[TICKSIZE];
	static float tick[TICKSIZE];
	int  tickTracker, tockTracker;
	bool tickPlay, tockPlay; 

	

	class SampleChannel *chanInput;

	

	int inputTracker;

	

	bool inToOut;

	pthread_mutex_t mutex_recs;
	pthread_mutex_t mutex_chans;
	pthread_mutex_t mutex_plugins;

private:

	int midiTCstep;      
	int midiTCrate;      
	int midiTCframes;
	int midiTCseconds;
	int midiTCminutes;
	int midiTChours;

	

	int getNewIndex();

	

	void sendMIDIsync();

	

	void sendMIDIrewind();
};

#endif
