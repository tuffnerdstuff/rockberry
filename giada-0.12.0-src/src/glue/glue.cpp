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




#include "../gui/elems/ge_waveform.h"
#include "../gui/elems/ge_mixed.h"
#include "../gui/elems/ge_channel.h"
#include "../gui/elems/ge_sampleChannel.h"
#include "../gui/elems/ge_waveTools.h"
#include "../gui/elems/ge_keyboard.h"
#include "../gui/dialogs/gd_mainWindow.h"
#include "../gui/dialogs/gd_editor.h"
#include "../gui/dialogs/gd_warnings.h"
#include "../utils/gui_utils.h"
#include "../utils/utils.h"
#include "../utils/log.h"
#include "../core/mixerHandler.h"
#include "../core/mixer.h"
#include "../core/recorder.h"
#include "../core/wave.h"
#include "../core/pluginHost.h"
#include "../core/channel.h"
#include "../core/sampleChannel.h"
#include "../core/midiChannel.h"
#include "../core/kernelMidi.h"
#include "../core/patch_DEPR_.h"
#include "../core/conf.h"
#include "glue.h"


extern gdMainWindow *mainWin;
extern Mixer	   		 G_Mixer;
extern Patch_DEPR_   G_Patch_DEPR_;
extern Conf	 	   		 G_Conf;
extern bool 		 		 G_audio_status;
#ifdef WITH_VST
extern PluginHost    G_PluginHost;
#endif


static bool __soloSession__ = false;


void glue_setBpm(const char *v1, const char *v2)
{
	char  buf[6];
	float value = atof(v1) + (atof(v2)/10);
	if (value < 20.0f)	{
		value = 20.0f;
		sprintf(buf, "20.0");
	}
	else
		sprintf(buf, "%s.%s", v1, !strcmp(v2, "") ? "0" : v2);

	

	float old_bpm = G_Mixer.bpm;
	G_Mixer.bpm = value;
	G_Mixer.updateFrameBars();

	

	recorder::updateBpm(old_bpm, value, G_Mixer.quanto);
	gu_refreshActionEditor();

	mainWin->timing->setBpm(buf);
	gLog("[glue] Bpm changed to %s (real=%f)\n", buf, G_Mixer.bpm);
}





void glue_setBeats(int beats, int bars, bool expand)
{
	

	int      oldvalue = G_Mixer.beats;
	unsigned oldfpb		= G_Mixer.totalFrames;

	if (beats > MAX_BEATS)
		G_Mixer.beats = MAX_BEATS;
	else if (beats < 1)
		G_Mixer.beats = 1;
	else
		G_Mixer.beats = beats;

	

	if (bars > G_Mixer.beats)
		G_Mixer.bars = G_Mixer.beats;
	else if (bars <= 0)
		G_Mixer.bars = 1;
	else if (beats % bars != 0) {
		G_Mixer.bars = bars + (beats % bars);
		if (beats % G_Mixer.bars != 0) 
			G_Mixer.bars = G_Mixer.bars - (beats % G_Mixer.bars);
	}
	else
		G_Mixer.bars = bars;

	G_Mixer.updateFrameBars();

	

	if (expand) {
		if (G_Mixer.beats > oldvalue)
			recorder::expand(oldfpb, G_Mixer.totalFrames);
		
		
	}

	mainWin->timing->setMeter(G_Mixer.beats, G_Mixer.bars);
	gu_refreshActionEditor();  
}





void glue_startStopSeq(bool gui)
{
	G_Mixer.running ? glue_stopSeq(gui) : glue_startSeq(gui);
}





void glue_startSeq(bool gui)
{
	G_Mixer.running = true;

	if (gui) {
#ifdef __linux__
		kernelAudio::jackStart();
#endif
	}

	if (G_Conf.midiSync == MIDI_SYNC_CLOCK_M) {
		kernelMidi::send(MIDI_START, -1, -1);
		kernelMidi::send(MIDI_POSITION_PTR, 0, 0);
	}

	if (gui) Fl::lock();
	mainWin->controller->updatePlay(1);
	if (gui) Fl::unlock();
}





void glue_stopSeq(bool gui) {

	mh_stopSequencer();

	if (G_Conf.midiSync == MIDI_SYNC_CLOCK_M)
		kernelMidi::send(MIDI_STOP, -1, -1);

#ifdef __linux__
	if (gui)
		kernelAudio::jackStop();
#endif

	

	if (recorder::active) {
		recorder::active = false;
		if (gui) Fl::lock();
		mainWin->controller->updateRecAction(0);
		if (gui) Fl::unlock();
	}

	

	if (G_Mixer.chanInput != NULL) {
		mh_stopInputRec();
		if (gui) Fl::lock();
		mainWin->controller->updateRecInput(0);
		if (gui) Fl::unlock();
	}

	if (gui) Fl::lock();
	mainWin->controller->updatePlay(0);
	if (gui) Fl::unlock();
}





void glue_rewindSeq() {
	mh_rewindSequencer();
	if (G_Conf.midiSync == MIDI_SYNC_CLOCK_M)
		kernelMidi::send(MIDI_POSITION_PTR, 0, 0);
}





void glue_startStopActionRec() {
	recorder::active ? glue_stopActionRec() : glue_startActionRec();
}





void glue_startActionRec() {
	if (G_audio_status == false)
		return;
	if (!G_Mixer.running)
		glue_startSeq();	        
	recorder::active = true;

	Fl::lock();
	mainWin->controller->updateRecAction(1);
	Fl::unlock();
}





void glue_stopActionRec() {

	

	recorder::active = false;
	recorder::sortActions();

	for (unsigned i=0; i<G_Mixer.channels.size(); i++)
		if (G_Mixer.channels.at(i)->type == CHANNEL_SAMPLE) {
			SampleChannel *ch = (SampleChannel*) G_Mixer.channels.at(i);
			if (ch->hasActions)
				ch->readActions = true;
			else
				ch->readActions = false;
			mainWin->keyboard->setChannelWithActions((gSampleChannel*)ch->guiChannel);
		}

	Fl::lock();
	mainWin->controller->updateRecAction(0);
	Fl::unlock();

	

	gu_refreshActionEditor();
}





void glue_startStopReadingRecs(SampleChannel *ch, bool gui) {
	if (ch->readActions)
		glue_stopReadingRecs(ch, gui);
	else
		glue_startReadingRecs(ch, gui);
}





void glue_startReadingRecs(SampleChannel *ch, bool gui) {
	if (G_Conf.treatRecsAsLoops)
		ch->recStatus = REC_WAITING;
	else
		ch->setReadActions(true);
	if (!gui) {
		gSampleChannel *gch = (gSampleChannel*)ch->guiChannel;
		if (gch->readActions) { 
			Fl::lock();
			gch->readActions->value(1);
			Fl::unlock();
		}
	}
}





void glue_stopReadingRecs(SampleChannel *ch, bool gui) {

	

	if (G_Conf.treatRecsAsLoops)
		ch->recStatus = REC_ENDING;
	else
		ch->setReadActions(false);
	if (!gui) {
		gSampleChannel *gch = (gSampleChannel*)ch->guiChannel;
		if (gch->readActions) {  
			Fl::lock();
			gch->readActions->value(0);
			Fl::unlock();
		}
	}
}





void glue_quantize(int val) {
	G_Mixer.quantize = val;
	G_Mixer.updateQuanto();
}





void glue_setChanVol(Channel *ch, float v, bool gui) {

	ch->volume = v;

	

	gdEditor *editor = (gdEditor*) gu_getSubwindow(mainWin, WID_SAMPLE_EDITOR);
	if (editor) {
		glue_setVolEditor(editor, (SampleChannel*) ch, v, false);
		Fl::lock();
		editor->volume->value(v);
		Fl::unlock();
	}

	if (!gui) {
		Fl::lock();
		ch->guiChannel->vol->value(v);
		Fl::unlock();
	}
}





void glue_setOutVol(float v, bool gui) {
	G_Mixer.outVol = v;
	if (!gui) {
		Fl::lock();
		mainWin->inOut->setOutVol(v);
		Fl::unlock();
	}
}





void glue_setInVol(float v, bool gui)
{
	G_Mixer.inVol = v;
	if (!gui) {
		Fl::lock();
		mainWin->inOut->setInVol(v);
		Fl::unlock();
	}
}





void glue_clearAllSamples()
{
	G_Mixer.running = false;
	for (unsigned i=0; i<G_Mixer.channels.size(); i++) {
		G_Mixer.channels.at(i)->empty();
		G_Mixer.channels.at(i)->guiChannel->reset();
	}
	recorder::init();
	return;
}





void glue_clearAllRecs()
{
	recorder::init();
	gu_updateControls();
}





void glue_resetToInitState(bool resetGui, bool createColumns)
{
	G_Patch_DEPR_.setDefault();
	G_Mixer.close();
	G_Mixer.init();
	recorder::init();
#ifdef WITH_VST
	G_PluginHost.freeAllStacks(&G_Mixer.channels, &G_Mixer.mutex_plugins);
#endif

	mainWin->keyboard->clear();
	if (createColumns)
		mainWin->keyboard->init();

	if (resetGui)
		gu_updateControls();
}





void glue_startStopMetronome(bool gui)
{
	G_Mixer.metronome = !G_Mixer.metronome;
	if (!gui) {
		Fl::lock();
		mainWin->controller->updateMetronome(G_Mixer.metronome);
		Fl::unlock();
	}
}





void glue_setBeginEndChannel(gdEditor *win, SampleChannel *ch, int b, int e, bool recalc, bool check)
{
	if (check) {
		if (e > ch->wave->size)
			e = ch->wave->size;
		if (b < 0)
			b = 0;
		if (b > ch->wave->size)
			b = ch->wave->size-2;
		if (b >= ch->end)
			b = ch->begin;
		if (e <= ch->begin)
			e = ch->end;
	}

	

	if (b == ch->begin && e == ch->end)
		return;

	

	char tmp[16];
	sprintf(tmp, "%d", b/2);
	win->chanStart->value(tmp);

	tmp[0] = '\0';
	sprintf(tmp, "%d", e/2);
	win->chanEnd->value(tmp);

	ch->setBegin(b);
	ch->setEnd(e);

	

	if (recalc) {
		win->waveTools->waveform->recalcPoints();	
		win->waveTools->waveform->redraw();
	}
}





void glue_setBoost(gdEditor *win, SampleChannel *ch, float val, bool numeric)
{
	if (numeric) {
		if (val > 20.0f)
			val = 20.0f;
		else if (val < 0.0f)
			val = 0.0f;

	  float linear = pow(10, (val / 20)); 

		ch->boost = linear;

		char buf[10];
		sprintf(buf, "%.2f", val);
		win->boostNum->value(buf);
		win->boostNum->redraw();

		win->boost->value(linear);
		win->boost->redraw();       
	}
	else {
		ch->boost = val;
		char buf[10];
		sprintf(buf, "%.2f", 20*log10(val));
		win->boostNum->value(buf);
		win->boostNum->redraw();
	}
}





void glue_setVolEditor(class gdEditor *win, SampleChannel *ch, float val, bool numeric)
{
	if (numeric) {
		if (val > 0.0f)
			val = 0.0f;
		else if (val < -60.0f)
			val = -INFINITY;

	  float linear = pow(10, (val / 20)); 

		ch->volume = linear;

		win->volume->value(linear);
		win->volume->redraw();

		char buf[10];
		if (val > -INFINITY)
			sprintf(buf, "%.2f", val);
		else
			sprintf(buf, "-inf");
		win->volumeNum->value(buf);
		win->volumeNum->redraw();

		ch->guiChannel->vol->value(linear);
		ch->guiChannel->vol->redraw();
	}
	else {
		ch->volume = val;

		float dbVal = 20 * log10(val);
		char buf[10];
		if (dbVal > -INFINITY)
			sprintf(buf, "%.2f", dbVal);
		else
			sprintf(buf, "-inf");

		win->volumeNum->value(buf);
		win->volumeNum->redraw();

		ch->guiChannel->vol->value(val);
		ch->guiChannel->vol->redraw();
	}
}





void glue_setMute(Channel *ch, bool gui)
{
	if (recorder::active && recorder::canRec(ch)) {
		if (!ch->mute)
			recorder::startOverdub(ch->index, ACTION_MUTES, G_Mixer.actualFrame);
		else
		 recorder::stopOverdub(G_Mixer.actualFrame);
	}

	ch->mute ? ch->unsetMute(false) : ch->setMute(false);

	if (!gui) {
		Fl::lock();
		ch->guiChannel->mute->value(ch->mute);
		Fl::unlock();
	}
}





void glue_setSoloOn(Channel *ch, bool gui)
{
	

	if (!__soloSession__) {
		for (unsigned i=0; i<G_Mixer.channels.size(); i++) {
			Channel *och = G_Mixer.channels.at(i);
			och->mute_s  = och->mute;
		}
		__soloSession__ = true;
	}

	ch->solo = !ch->solo;
	ch->sendMidiLsolo();

	

	for (unsigned i=0; i<G_Mixer.channels.size(); i++) {
		Channel *och = G_Mixer.channels.at(i);
		if (!och->solo && !och->mute) {
			och->setMute(false);
			Fl::lock();
			och->guiChannel->mute->value(true);
			Fl::unlock();
		}
	}

	if (ch->mute) {
		ch->unsetMute(false);
		Fl::lock();
		ch->guiChannel->mute->value(false);
		Fl::unlock();
	}

	if (!gui) {
		Fl::lock();
		ch->guiChannel->solo->value(1);
		Fl::unlock();
	}
}





void glue_setSoloOff(Channel *ch, bool gui)
{
	

	if (mh_uniqueSolo(ch)) {
		__soloSession__ = false;
		for (unsigned i=0; i<G_Mixer.channels.size(); i++) {
			Channel *och = G_Mixer.channels.at(i);
			if (och->mute_s) {
				och->setMute(false);
				Fl::lock();
				och->guiChannel->mute->value(true);
				Fl::unlock();
			}
			else {
				och->unsetMute(false);
				Fl::lock();
				och->guiChannel->mute->value(false);
				Fl::unlock();
			}
			och->mute_s = false;
		}
	}
	else {
		ch->setMute(false);
		Fl::lock();
		ch->guiChannel->mute->value(true);
		Fl::unlock();
	}

	ch->solo = !ch->solo;
	ch->sendMidiLsolo();

	if (!gui) {
		Fl::lock();
		ch->guiChannel->solo->value(0);
		Fl::unlock();
	}
}





void glue_setPanning(class gdEditor *win, SampleChannel *ch, float val)
{
	if (val < 1.0f) {
		ch->panLeft = 1.0f;
		ch->panRight= 0.0f + val;

		char buf[8];
		sprintf(buf, "%d L", abs((ch->panRight * 100.0f) - 100));
		win->panNum->value(buf);
	}
	else if (val == 1.0f) {
		ch->panLeft = 1.0f;
		ch->panRight= 1.0f;
	  win->panNum->value("C");
	}
	else {
		ch->panLeft = 2.0f - val;
		ch->panRight= 1.0f;

		char buf[8];
		sprintf(buf, "%d R", abs((ch->panLeft * 100.0f) - 100));
		win->panNum->value(buf);
	}
	win->panNum->redraw();
}





void glue_startStopInputRec(bool gui, bool alert)
{
	if (G_Mixer.chanInput == NULL) {
		if (!glue_startInputRec(gui)) {
			if (alert) gdAlert("No channels available for recording.");
			else       gLog("[glue] no channels available for recording\n");
		}
	}
	else
		glue_stopInputRec(gui);
}





int glue_startInputRec(bool gui)
{
	if (G_audio_status == false)
		return -1;

	SampleChannel *ch = mh_startInputRec();
	if (ch == NULL)	{                  
		Fl::lock();
		mainWin->controller->updateRecInput(0);
		Fl::unlock();
		return 0;
	}

	if (!G_Mixer.running) {
		glue_startSeq();
		Fl::lock();
		mainWin->controller->updatePlay(1);
		Fl::unlock();
	}

	glue_setChanVol(ch, 1.0f, false); 

	ch->guiChannel->mainButton->label(ch->wave->name.c_str());

	if (!gui) {
		Fl::lock();
		mainWin->controller->updateRecInput(1);
		Fl::unlock();
	}

	return 1;

}





int glue_stopInputRec(bool gui)
{
	SampleChannel *ch = mh_stopInputRec();

	if (ch->mode & (LOOP_BASIC | LOOP_ONCE | LOOP_REPEAT))
		ch->start(0, true);  

	if (!gui) {
		Fl::lock();
		mainWin->controller->updateRecInput(0);
		Fl::unlock();
	}

	return 1;
}





void glue_keyPress(Channel *ch, bool ctrl, bool shift)
{
	if (ch->type == CHANNEL_SAMPLE)
		glue_keyPress((SampleChannel*)ch, ctrl, shift);
	else
		glue_keyPress((MidiChannel*)ch, ctrl, shift);
}





void glue_keyRelease(Channel *ch, bool ctrl, bool shift)
{
	if (ch->type == CHANNEL_SAMPLE)
		glue_keyRelease((SampleChannel*)ch, ctrl, shift);
}





void glue_keyPress(MidiChannel *ch, bool ctrl, bool shift)
{
	if (ctrl)
		glue_setMute(ch);
	else
	if (shift)
		ch->kill(0);        
	else
		ch->start(0, true); 
}





void glue_keyPress(SampleChannel *ch, bool ctrl, bool shift)
{
	

	if (ctrl)
		glue_setMute(ch);

	

	else
	if (shift) {
		if (recorder::active) {
			if (G_Mixer.running) {
				ch->kill(0); 
				if (recorder::canRec(ch) && !(ch->mode & LOOP_ANY))   
					recorder::rec(ch->index, ACTION_KILLCHAN, G_Mixer.actualFrame);
			}
		}
		else {
			if (ch->hasActions) {
				if (G_Mixer.running || ch->status == STATUS_OFF)
					ch->readActions ? glue_stopReadingRecs(ch) : glue_startReadingRecs(ch);
				else
					ch->kill(0);  
			}
			else
				ch->kill(0);    
		}
	}

	

	else {

		

		if (G_Mixer.quantize == 0 &&
		    recorder::canRec(ch)  &&
	      !(ch->mode & LOOP_ANY))
		{
			if (ch->mode == SINGLE_PRESS)
				recorder::startOverdub(ch->index, ACTION_KEYS, G_Mixer.actualFrame);
			else
				recorder::rec(ch->index, ACTION_KEYPRESS, G_Mixer.actualFrame);
		}

		ch->start(0, true); 
	}

	
}





void glue_keyRelease(SampleChannel *ch, bool ctrl, bool shift)
{
	if (!ctrl && !shift) {
		ch->stop();

		

		if (ch->mode == SINGLE_PRESS && recorder::canRec(ch))
			recorder::stopOverdub(G_Mixer.actualFrame);
	}

	

}





void glue_setPitch(class gdEditor *win, SampleChannel *ch, float val, bool numeric)
{
	if (numeric) {
		if (val <= 0.0f)
			val = 0.1000f;
		if (val > 4.0f)
			val = 4.0000f;
		if (win)
			win->pitch->value(val);
	}

	ch->setPitch(val);

	if (win) {
		char buf[16];
		sprintf(buf, "%.4f", val);
		Fl::lock();
		win->pitchNum->value(buf);
		win->pitchNum->redraw();
		Fl::unlock();
	}
}







void glue_beatsMultiply()
{
	glue_setBeats(G_Mixer.beats*2, G_Mixer.bars, false);
}

void glue_beatsDivide()
{
	glue_setBeats(G_Mixer.beats/2, G_Mixer.bars, false);
}
