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




#ifndef SAMPLE_CHANNEL_H
#define SAMPLE_CHANNEL_H


#include <samplerate.h>
#include "channel.h"


class SampleChannel : public Channel
{
private:

	

	SRC_STATE *rsmp_state;
	SRC_DATA   rsmp_data;

	

	float *pChan;

	

	int frameRewind;

	

	int fillChan(float *dest, int start, int offset, bool rewind=true);

	

	void clearChan(float *dest, int start);

	

	void calcFadeoutStep();

	

	void calcVolumeEnv(int frame);

	
	 
	void generateUniqueSampleName();

public:

	SampleChannel(int bufferSize);
	~SampleChannel();

	void copy       (const Channel *src);
	void clear      ();
	void process    (float *buffer);
	void start      (int frame, bool doQuantize);
	void kill       (int frame);
	void empty      ();
	void stopBySeq  ();
	void stop       ();
	void rewind     ();
	void setMute    (bool internal);
	void unsetMute  (bool internal);
	void reset      (int frame);
	int  load       (const char *file);
	int  readPatch_DEPR_  (const char *file, int i);
  int  readPatch  (const string &basePath, int i);
	int  writePatch (int i, bool isProject);
	void quantize   (int index, int localFrame, int globalFrame);
	void onZero     (int frame);
	void onBar      (int frame);
	void parseAction(recorder::action *a, int localFrame, int globalFrame);

	

	void  setFadeIn  (bool internal);
	void  setFadeOut (int actionPostFadeout);
	void  setXFade   (int frame);

	

	void pushWave(class Wave *w);

	

	int getPosition();

	

	void sum(int frame, bool running);

	

	void setPitch(float v);

	

	void setBegin(unsigned v);
	void setEnd  (unsigned v);

	

	int save(const char *path);

	

	void hardStop(int frame);

	

	bool allocEmpty(int frames, int takeId);

	

	bool  canInputRec();

	

	void setReadActions(bool v);

	

	class  Wave *wave;
	int    tracker;         
	int    begin;
	int    end;
  float  pitch;
	float  boost;
	int    mode;            
	bool   qWait;           
	bool   fadeinOn;
	float  fadeinVol;
	bool   fadeoutOn;
	float  fadeoutVol;      
	int    fadeoutTracker;  
	float  fadeoutStep;     
  int    fadeoutType;     
  int		 fadeoutEnd;      

	

  bool   readActions;     

	

  uint32_t midiInReadActions;
  uint32_t midiInPitch;

	

	enum {
		DO_STOP   = 0x01,
		DO_MUTE   = 0x02,
		DO_MUTE_I = 0x04
	};

	

	enum {
		FADEOUT = 0x01,
		XFADE   = 0x02
	};
};

#endif
