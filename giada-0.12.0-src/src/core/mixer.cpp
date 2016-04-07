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




#include <math.h>
#include "../utils/log.h"
#include "../utils/gui_utils.h"
#include "mixer.h"
#include "init.h"
#include "wave.h"
#include "recorder.h"
#include "pluginHost.h"
#include "patch_DEPR_.h"
#include "conf.h"
#include "mixerHandler.h"
#include "channel.h"
#include "sampleChannel.h"
#include "midiChannel.h"
#include "kernelMidi.h"


extern Mixer 			 G_Mixer;
extern Patch_DEPR_ G_Patch_DEPR_;
extern Conf				 G_Conf;
#ifdef WITH_VST
extern PluginHost  G_PluginHost;
#endif


Mixer::Mixer()
	: vChanInput(NULL),
		vChanInToOut(NULL)
{}





Mixer::~Mixer() {}





#define TICKSIZE 38


float Mixer::tock[TICKSIZE] = {
	 0.059033,  0.117240,  0.173807,  0.227943,  0.278890,  0.325936,
	 0.368423,  0.405755,  0.437413,  0.462951,  0.482013,  0.494333,
	 0.499738,  0.498153,  0.489598,  0.474195,  0.452159,  0.423798,
	 0.389509,  0.349771,  0.289883,  0.230617,  0.173194,  0.118739,
	 0.068260,  0.022631, -0.017423, -0.051339,	-0.078721, -0.099345,
	-0.113163, -0.120295, -0.121028, -0.115804, -0.105209, -0.089954,
	-0.070862, -0.048844
};


float Mixer::tick[TICKSIZE] = {
	  0.175860,  0.341914,  0.488904,  0.608633,  0.694426,  0.741500,
	  0.747229,  0.711293,	0.635697,  0.524656,  0.384362,  0.222636,
	  0.048496, -0.128348, -0.298035, -0.451105, -0.579021, -0.674653,
	 -0.732667, -0.749830, -0.688924, -0.594091, -0.474481, -0.340160,
	 -0.201360, -0.067752,  0.052194,  0.151746,  0.226280,  0.273493,
	  0.293425,  0.288307,  0.262252,  0.220811,  0.170435,  0.117887,
	  0.069639,  0.031320
};





void Mixer::init()
{
	quanto      = 1;
	docross     = false;
	rewindWait  = false;
	running     = false;
	ready       = true;
	waitRec     = 0;
	actualFrame = 0;
	bpm 		    = DEFAULT_BPM;
	bars		    = DEFAULT_BARS;
	beats		    = DEFAULT_BEATS;
	quantize    = DEFAULT_QUANTIZE;
	metronome   = false;

	tickTracker = 0;
	tockTracker = 0;
	tickPlay    = false;
	tockPlay    = false;

	outVol       = DEFAULT_OUT_VOL;
	inVol        = DEFAULT_IN_VOL;
	peakOut      = 0.0f;
	peakIn	     = 0.0f;
	chanInput    = NULL;
	inputTracker = 0;

	actualBeat    = 0;

	midiTCstep    = 0;
	midiTCrate    = (G_Conf.samplerate / G_Conf.midiTCfps) * 2;  
	midiTCframes  = 0;
	midiTCseconds = 0;
	midiTCminutes = 0;
	midiTChours   = 0;

	
	

	vChanInput   = NULL;
	vChanInToOut = (float *) malloc(kernelAudio::realBufsize * 2 * sizeof(float));

	pthread_mutex_init(&mutex_recs, NULL);
	pthread_mutex_init(&mutex_chans, NULL);
	pthread_mutex_init(&mutex_plugins, NULL);

	updateFrameBars();
	rewind();
}





Channel *Mixer::addChannel(int type)
{
	Channel *ch;
	int bufferSize = kernelAudio::realBufsize*2;

	if (type == CHANNEL_SAMPLE)
		ch = new SampleChannel(bufferSize);
	else
		ch = new MidiChannel(bufferSize);

	while (true) {
		int lockStatus = pthread_mutex_trylock(&mutex_chans);
		if (lockStatus == 0) {
			channels.push_back(ch);
			pthread_mutex_unlock(&mutex_chans);
			break;
		}
	}

	ch->index = getNewIndex();
	gLog("[mixer] channel index=%d added, type=%d, total=%d\n", ch->index, ch->type, channels.size());
	return ch;
}





int Mixer::getNewIndex()
{
	

	if (channels.size() == 1)
		return 0;

	int index = 0;
	for (unsigned i=0; i<channels.size()-1; i++) {
		if (channels.at(i)->index > index)
			index = channels.at(i)->index;
		}
	index += 1;
	return index;
}





int Mixer::deleteChannel(Channel *ch)
{
	int index = -1;
	for (unsigned i=0; i<channels.size(); i++) {
		if (channels.at(i) == ch) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		gLog("[Mixer::deleteChannel] unable to find Channel %d for deletion!\n", ch->index);
		return 0;
	}

	int lockStatus;
	while (true) {
		lockStatus = pthread_mutex_trylock(&mutex_chans);
		if (lockStatus == 0) {
			channels.erase(channels.begin() + index);
			delete ch;
			pthread_mutex_unlock(&mutex_chans);
			return 1;
		}
		
		
	}
}





Channel *Mixer::getChannelByIndex(int index)
{
	for (unsigned i=0; i<channels.size(); i++)
		if (channels.at(i)->index == index)
			return channels.at(i);
	gLog("[mixer::getChannelByIndex] channel at index %d not found!\n", index);
	return NULL;
}





void Mixer::sendMIDIsync()
{
	if (G_Conf.midiSync == MIDI_SYNC_CLOCK_M) {
		if (actualFrame % (framesPerBeat/24) == 0)
			kernelMidi::send(MIDI_CLOCK, -1, -1);
	}
	else
	if (G_Conf.midiSync == MIDI_SYNC_MTC_M) {

		

		if (actualFrame % midiTCrate == 0) {

			

			if (midiTCframes % 2 == 0) {
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTCframes & 0x0F)  | 0x00, -1);
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTCframes >> 4)    | 0x10, -1);
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTCseconds & 0x0F) | 0x20, -1);
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTCseconds >> 4)   | 0x30, -1);
			}

			

			else {
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTCminutes & 0x0F) | 0x40, -1);
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTCminutes >> 4)   | 0x50, -1);
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTChours & 0x0F)   | 0x60, -1);
				kernelMidi::send(MIDI_MTC_QUARTER, (midiTChours >> 4)     | 0x70, -1);
			}

			midiTCframes++;

			

			if (midiTCframes > G_Conf.midiTCfps) {
				midiTCframes = 0;
				midiTCseconds++;
				if (midiTCseconds >= 60) {
					midiTCminutes++;
					midiTCseconds = 0;
					if (midiTCminutes >= 60) {
						midiTChours++;
						midiTCminutes = 0;
					}
				}
				
			}
		}
	}
}





void Mixer::sendMIDIrewind()
{
	midiTCframes  = 0;
	midiTCseconds = 0;
	midiTCminutes = 0;
	midiTChours   = 0;

	

	if (G_Conf.midiSync == MIDI_SYNC_MTC_M) {
		kernelMidi::send(MIDI_SYSEX, 0x7F, 0x00);  
		kernelMidi::send(0x01, 0x01, 0x00);        
		kernelMidi::send(0x00, 0x00, 0x00);        
		kernelMidi::send(MIDI_EOX, -1, -1);        
	}
}




int Mixer::masterPlay(
	void *out_buf, void *in_buf, unsigned n_frames,
	double streamTime, RtAudioStreamStatus status, void *userData)
{
	return G_Mixer.__masterPlay(out_buf, in_buf, n_frames);
}





int Mixer::__masterPlay(void *out_buf, void *in_buf, unsigned bufferFrames)
{
	if (!ready)
		return 0;

	float *outBuf = ((float *) out_buf);
	float *inBuf  = ((float *) in_buf);
	bufferFrames *= 2;     
	peakOut       = 0.0f;  
	peakIn        = 0.0f;  

	

	memset(outBuf, 0, sizeof(float) * bufferFrames);         
	memset(vChanInToOut, 0, sizeof(float) * bufferFrames);   

	pthread_mutex_lock(&mutex_chans);
	for (unsigned i=0; i<channels.size(); i++)
		if (channels.at(i)->type == CHANNEL_SAMPLE)
			((SampleChannel*)channels.at(i))->clear();
	pthread_mutex_unlock(&mutex_chans);

	for (unsigned j=0; j<bufferFrames; j+=2) {

		if (kernelAudio::inputEnabled) {

			

			if (inBuf[j] * inVol > peakIn)
				peakIn = inBuf[j] * inVol;

			

			if (inToOut) {
				vChanInToOut[j]   = inBuf[j]   * inVol;
				vChanInToOut[j+1] = inBuf[j+1] * inVol;
			}
		}

		

		if (running) {

			

			if (chanInput != NULL && kernelAudio::inputEnabled) {

				

				if (waitRec < G_Conf.delayComp)
					waitRec += 2;
				else {
					vChanInput[inputTracker]   += inBuf[j]   * inVol;
					vChanInput[inputTracker+1] += inBuf[j+1] * inVol;
					inputTracker += 2;
					if (inputTracker >= totalFrames)
						inputTracker = 0;
				}
			}

			

			if (quantize > 0 && quanto > 0) {
				if (actualFrame % (quanto) == 0) {   
					if (rewindWait) {
						rewindWait = false;
						rewind();
					}
					pthread_mutex_lock(&mutex_chans);
					for (unsigned k=0; k<channels.size(); k++)
						channels.at(k)->quantize(k, j, actualFrame);  
					pthread_mutex_unlock(&mutex_chans);
				}
			}

			

			if (actualFrame % framesPerBar == 0 && actualFrame != 0) {
				if (metronome)
					tickPlay = true;

				pthread_mutex_lock(&mutex_chans);
				for (unsigned k=0; k<channels.size(); k++)
					channels.at(k)->onBar(j);
				pthread_mutex_unlock(&mutex_chans);
			}

			

			if (actualFrame == 0) {
				pthread_mutex_lock(&mutex_chans);
				for (unsigned k=0; k<channels.size(); k++)
					channels.at(k)->onZero(j);
				pthread_mutex_unlock(&mutex_chans);
			}

			

			pthread_mutex_lock(&mutex_recs);
			for (unsigned y=0; y<recorder::frames.size(); y++) {
				if (recorder::frames.at(y) == actualFrame) {
					for (unsigned z=0; z<recorder::global.at(y).size(); z++) {
						int index   = recorder::global.at(y).at(z)->chan;
						Channel *ch = getChannelByIndex(index);
						ch->parseAction(recorder::global.at(y).at(z), j, actualFrame);
					}
					break;
				}
			}
			pthread_mutex_unlock(&mutex_recs);

			

			actualFrame += 2;

			

			if (actualFrame > totalFrames) {
				actualFrame = 0;
				actualBeat  = 0;
			}
			else
			if (actualFrame % framesPerBeat == 0 && actualFrame > 0) {
				actualBeat++;

				

				if (metronome && !tickPlay)
					tockPlay = true;
			}

			sendMIDIsync();

		} 

		

		pthread_mutex_lock(&mutex_chans);
		for (unsigned k=0; k<channels.size(); k++) {
			if (channels.at(k)->type == CHANNEL_SAMPLE)
				((SampleChannel*)channels.at(k))->sum(j, running);
		}
		pthread_mutex_unlock(&mutex_chans);

		
		

		if (tockPlay) {
			outBuf[j]   += tock[tockTracker];
			outBuf[j+1] += tock[tockTracker];
			tockTracker++;
			if (tockTracker >= TICKSIZE-1) {
				tockPlay    = false;
				tockTracker = 0;
			}
		}
		if (tickPlay) {
			outBuf[j]   += tick[tickTracker];
			outBuf[j+1] += tick[tickTracker];
			tickTracker++;
			if (tickTracker >= TICKSIZE-1) {
				tickPlay    = false;
				tickTracker = 0;
			}
		}
	} 


	

	pthread_mutex_lock(&mutex_chans);
	for (unsigned k=0; k<channels.size(); k++)
		channels.at(k)->process(outBuf);
	pthread_mutex_unlock(&mutex_chans);

	

#ifdef WITH_VST
	pthread_mutex_lock(&mutex_plugins);
	G_PluginHost.processStack(outBuf, PluginHost::MASTER_OUT);
	G_PluginHost.processStack(vChanInToOut, PluginHost::MASTER_IN);
	pthread_mutex_unlock(&mutex_plugins);
#endif

	

	for (unsigned j=0; j<bufferFrames; j+=2) {

		

		if (inToOut) {
			outBuf[j]   += vChanInToOut[j];
			outBuf[j+1] += vChanInToOut[j+1];
		}

		outBuf[j]   *= outVol;
		outBuf[j+1] *= outVol;

		

		if (outBuf[j] > peakOut)
			peakOut = outBuf[j];

		if (G_Conf.limitOutput) {
			if (outBuf[j] > 1.0f)
				outBuf[j] = 1.0f;
			else if (outBuf[j] < -1.0f)
				outBuf[j] = -1.0f;

			if (outBuf[j+1] > 1.0f)
				outBuf[j+1] = 1.0f;
			else if (outBuf[j+1] < -1.0f)
				outBuf[j+1] = -1.0f;
		}
	}

	return 0;
}





void Mixer::updateFrameBars()
{
	

	float seconds     = (60.0f / bpm) * beats;
	totalFrames       = G_Conf.samplerate * seconds * 2;
	framesPerBar      = totalFrames / bars;
	framesPerBeat     = totalFrames / beats;
	framesInSequencer = framesPerBeat * MAX_BEATS;

	

	if (totalFrames % 2 != 0)
		totalFrames--;
	if (framesPerBar % 2 != 0)
		framesPerBar--;
	if (framesPerBeat % 2 != 0)
		framesPerBeat--;

	updateQuanto();

	

	if (vChanInput != NULL)
		free(vChanInput);
	vChanInput = (float*) malloc(totalFrames * sizeof(float));
	if (!vChanInput)
		gLog("[Mixer] vChanInput realloc error!\n");
}





int Mixer::close()
{
	running = false;
	while (channels.size() > 0)
		deleteChannel(channels.at(0));

	if (vChanInput) {
		free(vChanInput);
		vChanInput = NULL;
	}
	if (vChanInToOut) {
		free(vChanInToOut);
		vChanInToOut = NULL;
	}
	return 1;
}





bool Mixer::isSilent()
{
	for (unsigned i=0; i<channels.size(); i++)
		if (channels.at(i)->status == STATUS_PLAY)
			return false;
	return true;
}





void Mixer::rewind()
{
	actualFrame = 0;
	actualBeat  = 0;

	if (running)
		for (unsigned i=0; i<channels.size(); i++)
			channels.at(i)->rewind();

	sendMIDIrewind();
}





void Mixer::updateQuanto()
{
	

	if (quantize != 0)
		quanto = framesPerBeat / quantize;
	if (quanto % 2 != 0)
		quanto++;
}





bool Mixer::hasLogicalSamples()
{
	for (unsigned i=0; i<channels.size(); i++)
		if (channels.at(i)->type == CHANNEL_SAMPLE)
			if (((SampleChannel*)channels.at(i))->wave)
				if (((SampleChannel*)channels.at(i))->wave->isLogical)
					return true;
	return false;
}





bool Mixer::hasEditedSamples()
{
	for (unsigned i=0; i<channels.size(); i++)
		if (channels.at(i)->type == CHANNEL_SAMPLE)
			if (((SampleChannel*)channels.at(i))->wave)
				if (((SampleChannel*)channels.at(i))->wave->isEdited)
					return true;
	return false;
}





bool Mixer::mergeVirtualInput()
{
	if (vChanInput == NULL) {
		gLog("[Mixer] virtual input channel not alloc'd\n");
		return false;
	}
	else {
#ifdef WITH_VST
		G_PluginHost.processStackOffline(vChanInput, PluginHost::MASTER_IN, 0, totalFrames);
#endif
		int numFrames = totalFrames*sizeof(float);
		memcpy(chanInput->wave->data, vChanInput, numFrames);
		memset(vChanInput, 0, numFrames); 
		return true;
	}
}
