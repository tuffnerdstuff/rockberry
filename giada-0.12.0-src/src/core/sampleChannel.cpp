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
#include "sampleChannel.h"
#include "patch_DEPR_.h"
#include "patch.h"
#include "conf.h"
#include "wave.h"
#include "pluginHost.h"
#include "waveFx.h"
#include "mixerHandler.h"
#include "kernelMidi.h"


extern Patch_DEPR_ G_Patch_DEPR_;
extern Patch       G_Patch;
extern Mixer       G_Mixer;
extern Conf        G_Conf;
#ifdef WITH_VST
extern PluginHost G_PluginHost;
#endif


using std::string;


SampleChannel::SampleChannel(int bufferSize)
	: Channel          (CHANNEL_SAMPLE, STATUS_EMPTY, bufferSize),
		frameRewind      (-1),
		wave             (NULL),
		tracker          (0),
		begin            (0),
		end              (0),
		pitch            (gDEFAULT_PITCH),
		boost            (1.0f),
		mode             (DEFAULT_CHANMODE),
		qWait	           (false),
		fadeinOn         (false),
		fadeinVol        (1.0f),
		fadeoutOn        (false),
		fadeoutVol       (1.0f),
		fadeoutTracker   (0),
		fadeoutStep      (DEFAULT_FADEOUT_STEP),
	  readActions      (true),
	  midiInReadActions(0x0),
	  midiInPitch      (0x0)
{
	rsmp_state = src_new(SRC_LINEAR, 2, NULL);
	pChan      = (float *) malloc(kernelAudio::realBufsize * 2 * sizeof(float));
}





SampleChannel::~SampleChannel()
{
	if (wave)
		delete wave;
	src_delete(rsmp_state);
	free(pChan);
}





void SampleChannel::copy(const Channel *_src)
{
	Channel::copy(_src);
	SampleChannel *src = (SampleChannel *) _src;
	tracker         = src->tracker;
	begin           = src->begin;
	end             = src->end;
	boost           = src->boost;
	mode            = src->mode;
	qWait           = src->qWait;
	fadeinOn        = src->fadeinOn;
	fadeinVol       = src->fadeinVol;
	fadeoutOn       = src->fadeoutOn;
	fadeoutVol      = src->fadeoutVol;
	fadeoutTracker  = src->fadeoutTracker;
	fadeoutStep     = src->fadeoutStep;
	fadeoutType     = src->fadeoutType;
	fadeoutEnd      = src->fadeoutEnd;
	setPitch(src->pitch);

	if (src->wave) {
		Wave *w = new Wave(*src->wave); 
		pushWave(w);
		generateUniqueSampleName();
	}
}





void SampleChannel::generateUniqueSampleName()
{
	string oldName = wave->name;
	int k = 1; 
	while (!mh_uniqueSamplename(this, wave->name.c_str())) {
		wave->updateName((oldName + "-" + gItoa(k)).c_str());
		k++;
	}
}





void SampleChannel::clear()
{
	

		memset(vChan, 0, sizeof(float) * bufferSize);
		memset(pChan, 0, sizeof(float) * bufferSize);

	if (status & (STATUS_PLAY | STATUS_ENDING)) {
		tracker = fillChan(vChan, tracker, 0);
		if (fadeoutOn && fadeoutType == XFADE) {
			gLog("[clear] filling pChan fadeoutTracker=%d\n", fadeoutTracker);
			fadeoutTracker = fillChan(pChan, fadeoutTracker, 0);
		}
	}
}





void SampleChannel::calcVolumeEnv(int frame)
{
	

	recorder::action *a0 = NULL;
	recorder::action *a1 = NULL;
	int res;

	

	res = recorder::getAction(index, ACTION_VOLUME, frame, &a0);
	if (res == 0)
		return;

	

	res = recorder::getNextAction(index, ACTION_VOLUME, frame, &a1);

	if (res == -1)
		res = recorder::getAction(index, ACTION_VOLUME, 0, &a1);

	volume_i = a0->fValue;
	volume_d = ((a1->fValue - a0->fValue) / ((a1->frame - a0->frame) / 2)) * 1.003f;
}





void SampleChannel::hardStop(int frame)
{
	if (frame != 0)        
		clearChan(vChan, frame);
	status = STATUS_OFF;
	sendMidiLplay();
	reset(frame);
}





void SampleChannel::onBar(int frame)
{
	
	
	

	if (mode == LOOP_REPEAT) {
		if (status == STATUS_PLAY)
			
			reset(frame);
	}
	else
	if (mode == LOOP_ONCE_BAR) {
		if (status == STATUS_WAIT) {
			status  = STATUS_PLAY;
			tracker = fillChan(vChan, tracker, frame);
			sendMidiLplay();
		}
	}
}





int SampleChannel::save(const char *path)
{
	return wave->writeData(path);
}





void SampleChannel::setBegin(unsigned v)
{
	begin   = v;
	tracker = begin;
}





void SampleChannel::setEnd(unsigned v)
{
	end = v;
}





void SampleChannel::setPitch(float v)
{
	pitch = v;
	rsmp_data.src_ratio = 1/pitch;

	

	if (status & (STATUS_OFF | STATUS_WAIT))
		src_set_ratio(rsmp_state, 1/pitch);
}





void SampleChannel::rewind()
{
	

	if (wave != NULL) {
		if ((mode & LOOP_ANY) || (recStatus == REC_READING && (mode & SINGLE_ANY)))
			reset(0);  
	}
}





void SampleChannel::parseAction(recorder::action *a, int localFrame, int globalFrame)
{
	if (readActions == false)
		return;

	switch (a->type) {
		case ACTION_KEYPRESS:
			if (mode & SINGLE_ANY)
				start(localFrame, false);
			break;
		case ACTION_KEYREL:
			if (mode & SINGLE_ANY)
				stop();
			break;
		case ACTION_KILLCHAN:
			if (mode & SINGLE_ANY)
				kill(localFrame);
			break;
		case ACTION_MUTEON:
			setMute(true);   
			break;
		case ACTION_MUTEOFF:
			unsetMute(true); 
			break;
		case ACTION_VOLUME:
			calcVolumeEnv(globalFrame);
			break;
	}
}





void SampleChannel::sum(int frame, bool running)
{
	if (wave == NULL || status & ~(STATUS_PLAY | STATUS_ENDING))
		return;

	if (frame != frameRewind) {

		

		if (running) {
			volume_i += volume_d;
			if (volume_i < 0.0f)
				volume_i = 0.0f;
			else
			if (volume_i > 1.0f)
				volume_i = 1.0f;
		}

		

		

		if (mute || mute_i) {
			vChan[frame]   = 0.0f;
			vChan[frame+1] = 0.0f;
		}
		else
		if (fadeinOn) {
			if (fadeinVol < 1.0f) {
				vChan[frame]   *= fadeinVol * volume_i;
				vChan[frame+1] *= fadeinVol * volume_i;
				fadeinVol += 0.01f;
			}
			else {
				fadeinOn  = false;
				fadeinVol = 0.0f;
			}
		}
		else
		if (fadeoutOn) {
			if (fadeoutVol > 0.0f) { 
				if (fadeoutType == XFADE) {
					vChan[frame]   *= volume_i;
					vChan[frame+1] *= volume_i;
					vChan[frame]    = pChan[frame]   * fadeoutVol * volume_i;
					vChan[frame+1]  = pChan[frame+1] * fadeoutVol * volume_i;
				}
				else {
					vChan[frame]   *= fadeoutVol * volume_i;
					vChan[frame+1] *= fadeoutVol * volume_i;
				}
				fadeoutVol -= fadeoutStep;
			}
			else {  
				fadeoutOn  = false;
				fadeoutVol = 1.0f;

				

				if (fadeoutType == XFADE) {
					qWait = false;
				}
				else {
					if (fadeoutEnd == DO_MUTE)
						mute = true;
					else
					if (fadeoutEnd == DO_MUTE_I)
						mute_i = true;
					else             
						hardStop(frame);
				}
			}
		}
		else {
			vChan[frame]   *= volume_i;
			vChan[frame+1] *= volume_i;
		}
	}
	else { 

		if (mode & (SINGLE_BASIC | SINGLE_PRESS | SINGLE_RETRIG) ||
			 (mode == SINGLE_ENDLESS && status == STATUS_ENDING)   ||
			 (mode & LOOP_ANY && !running))     
		{
			status = STATUS_OFF;
			sendMidiLplay();
		}

		

		if (mode & (LOOP_ONCE | LOOP_ONCE_BAR)) {
			if (status == STATUS_ENDING)
				status = STATUS_OFF;
			else
			if (status != STATUS_OFF)
				status = STATUS_WAIT;
		}

		

		reset(frame);
	}
}





void SampleChannel::onZero(int frame)
{
	if (wave == NULL)
		return;

	if (mode & LOOP_ANY) {

		

		if (status == STATUS_PLAY) {
			
			reset(frame);
		}
		else
		if (status == STATUS_ENDING)
			hardStop(frame);
	}

	if (status == STATUS_WAIT) { 
		status  = STATUS_PLAY;
		sendMidiLplay();
		tracker = fillChan(vChan, tracker, frame);
	}

	if (recStatus == REC_ENDING) {
		recStatus = REC_STOPPED;
		setReadActions(false);  
	}
	else
	if (recStatus == REC_WAITING) {
		recStatus = REC_READING;
		setReadActions(true);   
	}
}





void SampleChannel::quantize(int index, int localFrame, int globalFrame)
{
	

	if ((mode & LOOP_ANY) || !qWait)
		return;

	

	if (status == STATUS_OFF) {
		status  = STATUS_PLAY;
		sendMidiLplay();
		qWait   = false;
		tracker = fillChan(vChan, tracker, localFrame); 
	}
	else
		
		reset(localFrame);

	

	if (recorder::canRec(this)) {
		if (mode == SINGLE_PRESS)
			recorder::startOverdub(index, ACTION_KEYS, globalFrame);
		else
			recorder::rec(index, ACTION_KEYPRESS, globalFrame);
	}
}





int SampleChannel::getPosition()
{
	if (status & ~(STATUS_EMPTY | STATUS_MISSING | STATUS_OFF)) 
		return tracker - begin;
	else
		return -1;
}





void SampleChannel::setMute(bool internal)
{
	if (internal) {

		

		if (mute)
			mute_i = true;
		else {
			if (isPlaying())
				setFadeOut(DO_MUTE_I);
			else
				mute_i = true;
		}
	}
	else {

		

		if (mute_i)
			mute = true;
		else {

			

			if (isPlaying())
				setFadeOut(DO_MUTE);
			else
				mute = true;
		}
	}

	sendMidiLmute();
}





void SampleChannel::unsetMute(bool internal)
{
	if (internal) {
		if (mute)
			mute_i = false;
		else {
			if (isPlaying())
				setFadeIn(internal);
			else
				mute_i = false;
		}
	}
	else {
		if (mute_i)
			mute = false;
		else {
			if (isPlaying())
				setFadeIn(internal);
			else
				mute = false;
		}
	}

	sendMidiLmute();
}





void SampleChannel::calcFadeoutStep()
{
	if (end - tracker < (1 / DEFAULT_FADEOUT_STEP) * 2)
		fadeoutStep = ceil((end - tracker) / volume) * 2; 
	else
		fadeoutStep = DEFAULT_FADEOUT_STEP;
}





void SampleChannel::setReadActions(bool v)
{
	if (v)
		readActions = true;
	else {
		readActions = false;
		if (G_Conf.recsStopOnChanHalt)
			kill(0);  
	}
}





void SampleChannel::setFadeIn(bool internal)
{
	if (internal) mute_i = false;  
	else          mute   = false;
	fadeinOn  = true;
	fadeinVol = 0.0f;
}





void SampleChannel::setFadeOut(int actionPostFadeout)
{
	calcFadeoutStep();
	fadeoutOn   = true;
	fadeoutVol  = 1.0f;
	fadeoutType = FADEOUT;
	fadeoutEnd	= actionPostFadeout;
}





void SampleChannel::setXFade(int frame)
{
	gLog("[xFade] frame=%d tracker=%d\n", frame, tracker);

	calcFadeoutStep();
	fadeoutOn      = true;
	fadeoutVol     = 1.0f;
	fadeoutType    = XFADE;
	fadeoutTracker = fillChan(pChan, tracker, 0, false);
	reset(frame);
}







void SampleChannel::reset(int frame)
{
	
	tracker = begin;
	mute_i  = false;
	if (frame > 0 && status & (STATUS_PLAY | STATUS_ENDING))
		tracker = fillChan(vChan, tracker, frame);
}





void SampleChannel::empty()
{
	status = STATUS_OFF;
	if (wave) {
		delete wave;
		wave = NULL;
	}
	status = STATUS_EMPTY;
	sendMidiLplay();
}





void SampleChannel::pushWave(Wave *w)
{
	wave   = w;
	status = STATUS_OFF;
	sendMidiLplay();
	begin  = 0;
	end    = wave->size;
}





bool SampleChannel::allocEmpty(int frames, int takeId)
{
	Wave *w = new Wave();
	if (!w->allocEmpty(frames, G_Conf.samplerate))
		return false;

	char wname[32];
	sprintf(wname, "TAKE-%d", takeId);

	w->pathfile = gGetCurrentPath()+"/"+wname; 
	w->name     = wname;
	wave        = w;
	status      = STATUS_OFF;
	begin       = 0;
	end         = wave->size;

	sendMidiLplay();

	return true;
}





void SampleChannel::process(float *buffer)
{
#ifdef WITH_VST
	G_PluginHost.processStack(vChan, PluginHost::CHANNEL, this);
#endif

	for (int j=0; j<bufferSize; j+=2) {
		buffer[j]   += vChan[j]   * volume * panLeft  * boost;
		buffer[j+1] += vChan[j+1] * volume * panRight * boost;
	}
}





void SampleChannel::kill(int frame)
{
	if (wave != NULL && status != STATUS_OFF) {
		if (mute || mute_i || (status == STATUS_WAIT && mode & LOOP_ANY))
			hardStop(frame);
		else
			setFadeOut(DO_STOP);
	}
}





void SampleChannel::stopBySeq()
{
  

	if (mode & LOOP_ANY && status == STATUS_WAIT) {
		status = STATUS_OFF;
    return;
  }

  

	if (G_Conf.chansStopOnSeqHalt) {
    if ((mode & LOOP_ANY) || (hasActions && readActions && status == STATUS_PLAY))
      kill(0);
  }
}





void SampleChannel::stop()
{
	if (mode == SINGLE_PRESS && status == STATUS_PLAY) {
		if (mute || mute_i)
			hardStop(0);  
		else
			setFadeOut(DO_STOP);
	}
	else  
	if (mode == SINGLE_PRESS && qWait == true)
		qWait = false;
}





int SampleChannel::load(const char *file)
{
	if (strcmp(file, "") == 0 || gIsDir(file)) {
		gLog("[SampleChannel] file not specified\n");
		return SAMPLE_LEFT_EMPTY;
	}

	if (strlen(file) > FILENAME_MAX)
		return SAMPLE_PATH_TOO_LONG;

	Wave *w = new Wave();

	if (!w->open(file)) {
		gLog("[SampleChannel] %s: read error\n", file);
		delete w;
		return SAMPLE_READ_ERROR;
	}

	if (w->channels() > 2) {
		gLog("[SampleChannel] %s: unsupported multichannel wave\n", file);
		delete w;
		return SAMPLE_MULTICHANNEL;
	}

	if (!w->readData()) {
		delete w;
		return SAMPLE_READ_ERROR;
	}

	if (w->channels() == 1) 
		wfx_monoToStereo(w);

	if (w->rate() != G_Conf.samplerate) {
		gLog("[SampleChannel] input rate (%d) != system rate (%d), conversion needed\n",
				w->rate(), G_Conf.samplerate);
		w->resample(G_Conf.rsmpQuality, G_Conf.samplerate);
	}

	pushWave(w);
	generateUniqueSampleName();

	gLog("[SampleChannel] %s loaded in channel %d\n", file, index);
	return SAMPLE_LOADED_OK;
}





int SampleChannel::readPatch_DEPR_(const char *f, int i)
{
	int res = load(f);

		volume      = G_Patch_DEPR_.getVol(i);
		key         = G_Patch_DEPR_.getKey(i);
		index       = G_Patch_DEPR_.getIndex(i);
		mode        = G_Patch_DEPR_.getMode(i);
		mute        = G_Patch_DEPR_.getMute(i);
		mute_s      = G_Patch_DEPR_.getMute_s(i);
		solo        = G_Patch_DEPR_.getSolo(i);
		boost       = G_Patch_DEPR_.getBoost(i);
		panLeft     = G_Patch_DEPR_.getPanLeft(i);
		panRight    = G_Patch_DEPR_.getPanRight(i);
		readActions = G_Patch_DEPR_.getRecActive(i);
		recStatus   = readActions ? REC_READING : REC_STOPPED;

		readPatchMidiIn_DEPR_(i);
		midiInReadActions = G_Patch_DEPR_.getMidiValue(i, "InReadActions");
		midiInPitch       = G_Patch_DEPR_.getMidiValue(i, "InPitch");
		readPatchMidiOut_DEPR_(i);

	if (res == SAMPLE_LOADED_OK) {
		setBegin(G_Patch_DEPR_.getBegin(i));
		setEnd  (G_Patch_DEPR_.getEnd(i, wave->size));
		setPitch(G_Patch_DEPR_.getPitch(i));
	}
	else {
		
		
		
		

		if (res == SAMPLE_LEFT_EMPTY)
			status = STATUS_EMPTY;
		else
		if (res == SAMPLE_READ_ERROR)
			status = STATUS_MISSING;
		sendMidiLplay();
	}

	return res;
}





int SampleChannel::readPatch(const string &basePath, int i)
{
	

	Channel::readPatch("", i);

	Patch::channel_t *pch = &G_Patch.channels.at(i);

	mode              = pch->mode;
	boost             = pch->boost;
	readActions       = pch->recActive;
	recStatus         = readActions ? REC_READING : REC_STOPPED;
	midiInReadActions = pch->midiInReadActions;
	midiInPitch       = pch->midiInPitch;

	int res = load((basePath + pch->samplePath).c_str());
	if (res == SAMPLE_LOADED_OK) {
		setBegin(pch->begin);
		setEnd  (pch->end);
		setPitch(pch->pitch);
	}
	else {
		if (res == SAMPLE_LEFT_EMPTY)
			status = STATUS_EMPTY;
		else
		if (res == SAMPLE_READ_ERROR)
			status = STATUS_MISSING;
		sendMidiLplay();  
	}

	return res;
}





bool SampleChannel::canInputRec()
	{
	return wave == NULL;
}





void SampleChannel::start(int frame, bool doQuantize)
{
	switch (status)	{
		case STATUS_EMPTY:
		case STATUS_MISSING:
		case STATUS_WRONG:
		{
			return;
		}

		case STATUS_OFF:
		{
			if (mode & LOOP_ANY) {
				status = STATUS_WAIT;
				sendMidiLplay();
			}
			else {
				if (G_Mixer.quantize > 0 && G_Mixer.running && doQuantize)
					qWait = true;
				else {

					

					status = STATUS_PLAY;
					sendMidiLplay();
					if (frame != 0)
						tracker = fillChan(vChan, tracker, frame);
				}
			}
			break;
		}

		case STATUS_PLAY:
		{
			if (mode == SINGLE_BASIC)
				setFadeOut(DO_STOP);
			else
			if (mode == SINGLE_RETRIG) {
				if (G_Mixer.quantize > 0 && G_Mixer.running && doQuantize)
					qWait = true;
				else
					reset(frame);
			}
			else
			if (mode & (LOOP_ANY | SINGLE_ENDLESS)) {
				status = STATUS_ENDING;
				sendMidiLplay();
			}
			break;
		}

		case STATUS_WAIT:
		{
			status = STATUS_OFF;
			sendMidiLplay();
			break;
		}

		case STATUS_ENDING:
		{
			status = STATUS_PLAY;
			sendMidiLplay();
			break;
		}
	}
}





int SampleChannel::writePatch(int i, bool isProject)
{
	int pchIndex = Channel::writePatch(i, isProject);
	Patch::channel_t *pch = &G_Patch.channels.at(pchIndex);

	if (wave != NULL) {
		pch->samplePath = wave->pathfile;
		if (isProject)
			pch->samplePath = gBasename(wave->pathfile);  
	}
	else
		pch->samplePath = "";

	pch->mode              = mode;
	pch->begin             = begin;
	pch->end               = end;
	pch->boost             = boost;
	pch->recActive         = readActions;
	pch->pitch             = pitch;
	pch->midiInReadActions = midiInReadActions;
	pch->midiInPitch       = midiInPitch;

	return 0;
}





void SampleChannel::clearChan(float *dest, int start)
{
	memset(dest+start, 0, sizeof(float)*(bufferSize-start));
}





int SampleChannel::fillChan(float *dest, int start, int offset, bool rewind)
{
	int position;  

	if (pitch == 1.0f) {

		

		if (start+bufferSize-offset <= end) {
			memcpy(dest+offset, wave->data+start, (bufferSize-offset)*sizeof(float));
			position = start+bufferSize-offset;
			if (rewind)
				frameRewind = -1;
		}

		

		else {
			memcpy(dest+offset, wave->data+start, (end-start)*sizeof(float));
			position = end;
			if (rewind)
				frameRewind = end-start+offset;
		}
	}
	else {

		rsmp_data.data_in       = wave->data+start;         
		rsmp_data.input_frames  = (end-start)/2;            
		rsmp_data.data_out      = dest+offset;              
		rsmp_data.output_frames = (bufferSize-offset)/2;    
		rsmp_data.end_of_input  = false;

		src_process(rsmp_state, &rsmp_data);
		int gen = rsmp_data.output_frames_gen*2;            

		position = start + rsmp_data.input_frames_used*2;   

		if (rewind) {
			if (gen == bufferSize-offset)
				frameRewind = -1;
			else
				frameRewind = gen+offset;
		}
	}
	return position;
}
