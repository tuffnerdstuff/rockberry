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




#include "../../core/conf.h"
#include "../../core/midiMapConf.h"
#include "../../core/patch_DEPR_.h"
#include "../../core/kernelAudio.h"
#include "../../core/kernelMidi.h"
#include "../../core/pluginHost.h"
#include "../../utils/gui_utils.h"
#include "../../utils/log.h"
#include "../elems/ge_mixed.h"
#include "gd_config.h"
#include "gd_keyGrabber.h"
#include "gd_devInfo.h"
#include "gd_browser.h"


extern Patch_DEPR_ G_Patch_DEPR_;
extern Conf	       G_Conf;
extern bool        G_audio_status;
extern MidiMapConf G_MidiMap;

#ifdef WITH_VST
extern PluginHost G_PluginHost;
#endif


using std::string;


gTabMisc::gTabMisc(int X, int Y, int W, int H)
	: Fl_Group(X, Y, W, H, "Misc")
{
	begin();
	debugMsg = new gChoice(x()+92,  y()+9,  253, 20, "Debug messages");
	end();

	debugMsg->add("(disabled)");
	debugMsg->add("To standard output");
	debugMsg->add("To file");

	labelsize(GUI_FONT_SIZE_BASE);

	switch (G_Conf.logMode) {
		case LOG_MODE_MUTE:
			debugMsg->value(0);
			break;
		case LOG_MODE_STDOUT:
			debugMsg->value(1);
			break;
		case LOG_MODE_FILE:
			debugMsg->value(2);
			break;
	}
}





void gTabMisc::save()
{
	switch(debugMsg->value()) {
		case 0:
			G_Conf.logMode = LOG_MODE_MUTE;
			break;
		case 1:
			G_Conf.logMode = LOG_MODE_STDOUT;
			break;
		case 2:
			G_Conf.logMode = LOG_MODE_FILE;
			break;
	}
}







gTabAudio::gTabAudio(int X, int Y, int W, int H)
	: Fl_Group(X, Y, W, H, "Sound System")
{
	begin();
	soundsys    = new gChoice(x()+92,  y()+9,  253, 20, "System");
	buffersize  = new gChoice(x()+92,  y()+37, 55,  20, "Buffer size");
	samplerate  = new gChoice(x()+290, y()+37, 55,  20, "Sample rate");
	sounddevOut = new gChoice(x()+92,  y()+65, 225, 20, "Output device");
	devOutInfo  = new gClick (x()+325, y()+65, 20,  20, "?");
	channelsOut = new gChoice(x()+92,  y()+93, 55,  20, "Output channels");
	limitOutput = new gCheck (x()+155, y()+97, 55,  20, "Limit output");
	sounddevIn  = new gChoice(x()+92,  y()+121, 225, 20, "Input device");
	devInInfo   = new gClick (x()+325, y()+121, 20,  20, "?");
	channelsIn  = new gChoice(x()+92,  y()+149, 55,  20, "Input channels");
	delayComp   = new gInput (x()+290, y()+149, 55,  20, "Rec delay comp.");
	rsmpQuality = new gChoice(x()+92, y()+177, 253, 20, "Resampling");
                new gBox(x(), rsmpQuality->y()+rsmpQuality->h()+8, w(), 92,
										"Restart Giada for the changes to take effect.");
	end();
	labelsize(GUI_FONT_SIZE_BASE);

	soundsys->add("(none)");

#if defined(__linux__)

	if (kernelAudio::hasAPI(RtAudio::LINUX_ALSA))
		soundsys->add("ALSA");
	if (kernelAudio::hasAPI(RtAudio::UNIX_JACK))
		soundsys->add("Jack");
	if (kernelAudio::hasAPI(RtAudio::LINUX_PULSE))
		soundsys->add("PulseAudio");

	switch (G_Conf.soundSystem) {
		case SYS_API_NONE:
			soundsys->showItem("(none)");
			break;
		case SYS_API_ALSA:
			soundsys->showItem("ALSA");
			break;
		case SYS_API_JACK:
			soundsys->showItem("Jack");
			buffersize->deactivate();
			samplerate->deactivate();
			break;
		case SYS_API_PULSE:
			soundsys->showItem("PulseAudio");
			break;
	}

#elif defined(_WIN32)

	if (kernelAudio::hasAPI(RtAudio::WINDOWS_DS))
		soundsys->add("DirectSound");
	if (kernelAudio::hasAPI(RtAudio::WINDOWS_ASIO))
		soundsys->add("ASIO");

	switch (G_Conf.soundSystem) {
		case SYS_API_NONE:
			soundsys->showItem("(none)");
			break;
		case SYS_API_DS:
			soundsys->showItem("DirectSound");
			break;
		case SYS_API_ASIO:
			soundsys->showItem("ASIO");
			break;
	}

#elif defined (__APPLE__)

	if (kernelAudio::hasAPI(RtAudio::MACOSX_CORE))
		soundsys->add("CoreAudio");

	switch (G_Conf.soundSystem) {
		case SYS_API_NONE:
			soundsys->showItem("(none)");
			break;
		case SYS_API_CORE:
			soundsys->showItem("CoreAudio");
			break;
	}

#endif

	soundsysInitValue = soundsys->value();

	soundsys->callback(cb_deactivate_sounddev, (void*)this);

	sounddevIn->callback(cb_fetchInChans, this);
	sounddevOut->callback(cb_fetchOutChans, this);

	devOutInfo->callback(cb_showOutputInfo, this);
	devInInfo->callback(cb_showInputInfo, this);

	if (G_Conf.soundSystem != SYS_API_NONE) {
		fetchSoundDevs();
		fetchOutChans(sounddevOut->value());
		fetchInChans(sounddevIn->value());

		
		

		int nfreq = kernelAudio::getTotalFreqs(sounddevOut->value());
		for (int i=0; i<nfreq; i++) {
			int freq = kernelAudio::getFreq(sounddevOut->value(), i);
			samplerate->add(gItoa(freq).c_str());
			if (freq == G_Conf.samplerate)
				samplerate->value(i);
		}
	}
	else {
		sounddevIn->deactivate();
		sounddevOut->deactivate();
		channelsIn->deactivate();
		channelsOut->deactivate();
		devOutInfo->deactivate();
		devInInfo->deactivate();
		samplerate->deactivate();
	}

	buffersize->add("8");
	buffersize->add("16");
	buffersize->add("32");
	buffersize->add("64");
	buffersize->add("128");
	buffersize->add("256");
	buffersize->add("512");
	buffersize->add("1024");
	buffersize->add("2048");
	buffersize->add("4096");
	buffersize->showItem(gItoa(G_Conf.buffersize).c_str());

	rsmpQuality->add("Sinc best quality (very slow)");
	rsmpQuality->add("Sinc medium quality (slow)");
	rsmpQuality->add("Sinc basic quality (medium)");
	rsmpQuality->add("Zero Order Hold (fast)");
	rsmpQuality->add("Linear (very fast)");
	rsmpQuality->value(G_Conf.rsmpQuality);

	delayComp->value(gItoa(G_Conf.delayComp).c_str());
	delayComp->type(FL_INT_INPUT);
	delayComp->maximum_size(5);

	limitOutput->value(G_Conf.limitOutput);
}





void gTabAudio::cb_deactivate_sounddev(Fl_Widget *w, void *p) { ((gTabAudio*)p)->__cb_deactivate_sounddev(); }
void gTabAudio::cb_fetchInChans(Fl_Widget *w, void *p)        { ((gTabAudio*)p)->__cb_fetchInChans(); }
void gTabAudio::cb_fetchOutChans(Fl_Widget *w, void *p)       { ((gTabAudio*)p)->__cb_fetchOutChans(); }
void gTabAudio::cb_showInputInfo(Fl_Widget *w, void *p)       { ((gTabAudio*)p)->__cb_showInputInfo(); }
void gTabAudio::cb_showOutputInfo(Fl_Widget *w, void *p)      { ((gTabAudio*)p)->__cb_showOutputInfo(); }





void gTabAudio::__cb_fetchInChans()
{
	fetchInChans(sounddevIn->value());
	channelsIn->value(0);
}





void gTabAudio::__cb_fetchOutChans()
{
	fetchOutChans(sounddevOut->value());
	channelsOut->value(0);
}





void gTabAudio::__cb_showInputInfo()
{
	unsigned dev = kernelAudio::getDeviceByName(sounddevIn->text(sounddevIn->value()));
	new gdDevInfo(dev);
}





void gTabAudio::__cb_showOutputInfo()
{
	unsigned dev = kernelAudio::getDeviceByName(sounddevOut->text(sounddevOut->value()));
	new gdDevInfo(dev);
}





void gTabAudio::__cb_deactivate_sounddev()
{
	

	if (soundsysInitValue == soundsys->value() && soundsysInitValue != 0) {
		sounddevOut->clear();
		sounddevIn->clear();

		fetchSoundDevs();

		

		fetchOutChans(sounddevOut->value());
		sounddevOut->activate();
		channelsOut->activate();

		

		fetchInChans(sounddevIn->value());
		sounddevIn->activate();
		samplerate->activate();
	}
	else {
		sounddevOut->deactivate();
		sounddevOut->clear();
		sounddevOut->add("-- restart to fetch device(s) --");
		sounddevOut->value(0);
		channelsOut->deactivate();
		devOutInfo->deactivate();
		samplerate->deactivate();

		sounddevIn->deactivate();
		sounddevIn->clear();
		sounddevIn->add("-- restart to fetch device(s) --");
		sounddevIn->value(0);
		channelsIn->deactivate();
		devInInfo->deactivate();
	}
}




void gTabAudio::fetchInChans(int menuItem)
{
	

	if (menuItem == 0) {
		devInInfo ->deactivate();
		channelsIn->deactivate();
		delayComp ->deactivate();
		return;
	}

	devInInfo ->activate();
	channelsIn->activate();
	delayComp ->activate();

	channelsIn->clear();

	unsigned dev = kernelAudio::getDeviceByName(sounddevIn->text(sounddevIn->value()));
	unsigned chs = kernelAudio::getMaxInChans(dev);

	if (chs == 0) {
		channelsIn->add("none");
		channelsIn->value(0);
		return;
	}
	for (unsigned i=0; i<chs; i+=2) {
		char str[16];
		sprintf(str, "%d-%d", (i+1), (i+2));
		channelsIn->add(str);
	}
	channelsIn->value(G_Conf.channelsIn);
}





void gTabAudio::fetchOutChans(int menuItem)
{
	channelsOut->clear();

	unsigned dev = kernelAudio::getDeviceByName(sounddevOut->text(sounddevOut->value()));
	unsigned chs = kernelAudio::getMaxOutChans(dev);

	if (chs == 0) {
		channelsOut->add("none");
		channelsOut->value(0);
		return;
	}
	for (unsigned i=0; i<chs; i+=2) {
		char str[16];
		sprintf(str, "%d-%d", (i+1), (i+2));
		channelsOut->add(str);
	}
	channelsOut->value(G_Conf.channelsOut);
}





int gTabAudio::findMenuDevice(gChoice *m, int device)
{
	if (device == -1)
		return 0;

	if (G_audio_status == false)
		return 0;

	for (int i=0; i<m->size(); i++) {
		if (kernelAudio::getDeviceName(device) == "")
			continue;
		if (m->text(i) == NULL)
			continue;
		if (m->text(i) == kernelAudio::getDeviceName(device))
			return i;
	}

	return 0;
}





void gTabAudio::fetchSoundDevs()
{
	if (kernelAudio::numDevs == 0) {
		sounddevOut->add("-- no devices found --");
		sounddevOut->value(0);
		sounddevIn->add("-- no devices found --");
		sounddevIn->value(0);
		devInInfo ->deactivate();
		devOutInfo->deactivate();
	}
	else {

		devInInfo ->activate();
		devOutInfo->activate();

		

		sounddevIn->add("(disabled)");

		for (unsigned i=0; i<kernelAudio::numDevs; i++) {

			

			string tmp = kernelAudio::getDeviceName(i);
			for (unsigned k=0; k<tmp.size(); k++)
				if (tmp[k] == '/' || tmp[k] == '|' || tmp[k] == '&' || tmp[k] == '_')
					tmp[k] = '-';

			

			if (kernelAudio::getMaxOutChans(i) > 0)
				sounddevOut->add(tmp.c_str());

			if (kernelAudio::getMaxInChans(i) > 0)
				sounddevIn->add(tmp.c_str());
		}

		

		if (sounddevOut->size() == 0) {
			sounddevOut->add("-- no devices found --");
			sounddevOut->value(0);
			devOutInfo->deactivate();
		}
		else {
			int outMenuValue = findMenuDevice(sounddevOut, G_Conf.soundDeviceOut);
			sounddevOut->value(outMenuValue);
		}

		if (sounddevIn->size() == 0) {
			sounddevIn->add("-- no devices found --");
			sounddevIn->value(0);
			devInInfo->deactivate();
		}
		else {
			int inMenuValue = findMenuDevice(sounddevIn, G_Conf.soundDeviceIn);
			sounddevIn->value(inMenuValue);
		}
	}
}





void gTabAudio::save()
{
	string text = soundsys->text(soundsys->value());

	if (text == "(none)") {
		G_Conf.soundSystem = SYS_API_NONE;
		return;
	}

#if defined(__linux__)

	else if (text == "ALSA")
		G_Conf.soundSystem = SYS_API_ALSA;
	else if (text == "Jack")
		G_Conf.soundSystem = SYS_API_JACK;
	else if (text == "PulseAudio")
		G_Conf.soundSystem = SYS_API_PULSE;

#elif defined(_WIN32)

	else if (text == "DirectSound")
		G_Conf.soundSystem = SYS_API_DS;
	else if (text == "ASIO")
		G_Conf.soundSystem = SYS_API_ASIO;

#elif defined (__APPLE__)

	else if (text == "CoreAudio")
		G_Conf.soundSystem = SYS_API_CORE;

#endif

	

	G_Conf.soundDeviceOut = kernelAudio::getDeviceByName(sounddevOut->text(sounddevOut->value()));
	G_Conf.soundDeviceIn  = kernelAudio::getDeviceByName(sounddevIn->text(sounddevIn->value()));
	G_Conf.channelsOut    = channelsOut->value();
	G_Conf.channelsIn     = channelsIn->value();
	G_Conf.limitOutput    = limitOutput->value();
	G_Conf.rsmpQuality    = rsmpQuality->value();

	

	if (G_Conf.soundDeviceOut == -1)
		G_Conf.soundDeviceOut = 0;

	int bufsize = atoi(buffersize->text());
	if (bufsize % 2 != 0) bufsize++;
	if (bufsize < 8)		  bufsize = 8;
	if (bufsize > 8192)		bufsize = 8192;
	G_Conf.buffersize = bufsize;

	const Fl_Menu_Item *i = NULL;
	i = samplerate->mvalue(); 
	if (i)
		G_Conf.samplerate = atoi(i->label());

	G_Conf.delayComp = atoi(delayComp->value());
}







gTabMidi::gTabMidi(int X, int Y, int W, int H)
	: Fl_Group(X, Y, W, H, "MIDI")
{
	begin();
	system	  = new gChoice(x()+92, y()+9, 253, 20, "System");
	portOut	  = new gChoice(x()+92, system->y()+system->h()+8, 253, 20, "Output port");
	portIn	  = new gChoice(x()+92, portOut->y()+portOut->h()+8, 253, 20, "Input port");
	noNoteOff = new gCheck (x()+92, portIn->y()+portIn->h()+8, 253, 20, "Device does not send NoteOff");
	midiMap	  = new gChoice(x()+92, noNoteOff->y()+noNoteOff->h(), 253, 20, "Output Midi Map");
	sync	    = new gChoice(x()+92, midiMap->y()+midiMap->h()+8, 253, 20, "Sync");
	new gBox(x(), sync->y()+sync->h()+8, w(), h()-125, "Restart Giada for the changes to take effect.");
	end();

	labelsize(GUI_FONT_SIZE_BASE);

	system->callback(cb_changeSystem, (void*)this);

	fetchSystems();
	fetchOutPorts();
	fetchInPorts();
	fetchMidiMaps();

	noNoteOff->value(G_Conf.noNoteOff);

	sync->add("(disabled)");
	sync->add("MIDI Clock (master)");
	sync->add("MTC (master)");
	if      (G_Conf.midiSync == MIDI_SYNC_NONE)
		sync->value(0);
	else if (G_Conf.midiSync == MIDI_SYNC_CLOCK_M)
		sync->value(1);
	else if (G_Conf.midiSync == MIDI_SYNC_MTC_M)
		sync->value(2);

	systemInitValue = system->value();
}





void gTabMidi::fetchOutPorts() {

	if (kernelMidi::numOutPorts == 0) {
		portOut->add("-- no ports found --");
		portOut->value(0);
		portOut->deactivate();
	}
	else {

		portOut->add("(disabled)");

		for (unsigned i=0; i<kernelMidi::numOutPorts; i++)
			portOut->add(gu_removeFltkChars(kernelMidi::getOutPortName(i)).c_str());

		portOut->value(G_Conf.midiPortOut+1);    
	}
}




void gTabMidi::fetchInPorts()
{
	if (kernelMidi::numInPorts == 0) {
		portIn->add("-- no ports found --");
		portIn->value(0);
		portIn->deactivate();
	}
	else {

		portIn->add("(disabled)");

		for (unsigned i=0; i<kernelMidi::numInPorts; i++)
			portIn->add(gu_removeFltkChars(kernelMidi::getInPortName(i)).c_str());

		portIn->value(G_Conf.midiPortIn+1);    
	}
}





void gTabMidi::fetchMidiMaps()
{
	if (G_MidiMap.maps.size() == 0) {
		midiMap->add("(no MIDI maps available)");
		midiMap->value(0);
		midiMap->deactivate();
		return;
	}
	for (unsigned i=0; i<G_MidiMap.maps.size(); i++) {
		const char *imap = G_MidiMap.maps.at(i).c_str();
		midiMap->add(imap);
		if (G_Conf.midiMapPath == imap)
			midiMap->value(i);
	}
}





void gTabMidi::save()
{
	string text = system->text(system->value());

	if      (text == "ALSA")
		G_Conf.midiSystem = RtMidi::LINUX_ALSA;
	else if (text == "Jack")
		G_Conf.midiSystem = RtMidi::UNIX_JACK;
	else if (text == "Multimedia MIDI")
		G_Conf.midiSystem = RtMidi::WINDOWS_MM;
	else if (text == "OSX Core MIDI")
		G_Conf.midiSystem = RtMidi::MACOSX_CORE;

	G_Conf.midiPortOut = portOut->value()-1;   
	G_Conf.midiPortIn  = portIn->value()-1;    

	G_Conf.noNoteOff   = noNoteOff->value();
	G_Conf.midiMapPath = G_MidiMap.maps.size() == 0 ? "" : midiMap->text(midiMap->value());

	if      (sync->value() == 0)
		G_Conf.midiSync = MIDI_SYNC_NONE;
	else if (sync->value() == 1)
		G_Conf.midiSync = MIDI_SYNC_CLOCK_M;
	else if (sync->value() == 2)
		G_Conf.midiSync = MIDI_SYNC_MTC_M;
}





void gTabMidi::fetchSystems()
{
#if defined(__linux__)

	if (kernelMidi::hasAPI(RtMidi::LINUX_ALSA))
		system->add("ALSA");
	if (kernelMidi::hasAPI(RtMidi::UNIX_JACK))
		system->add("Jack");

#elif defined(_WIN32)

	if (kernelMidi::hasAPI(RtMidi::WINDOWS_MM))
		system->add("Multimedia MIDI");

#elif defined (__APPLE__)

	system->add("OSX Core MIDI");

#endif

	switch (G_Conf.midiSystem) {
		case RtMidi::LINUX_ALSA:  system->showItem("ALSA"); break;
		case RtMidi::UNIX_JACK:   system->showItem("Jack"); break;
		case RtMidi::WINDOWS_MM:  system->showItem("Multimedia MIDI"); break;
		case RtMidi::MACOSX_CORE: system->showItem("OSX Core MIDI"); break;
		default: system->value(0); break;
	}
}





void gTabMidi::cb_changeSystem(Fl_Widget *w, void *p) { ((gTabMidi*)p)->__cb_changeSystem(); }





void gTabMidi::__cb_changeSystem()
{
	

	if (systemInitValue == system->value()) {
		portOut->clear();
		fetchOutPorts();
		portOut->activate();
		portIn->clear();
		fetchInPorts();
		portIn->activate();
		noNoteOff->activate();
		sync->activate();
	}
	else {
		portOut->deactivate();
		portOut->clear();
		portOut->add("-- restart to fetch device(s) --");
		portOut->value(0);
		portIn->deactivate();
		portIn->clear();
		portIn->add("-- restart to fetch device(s) --");
		portIn->value(0);
		noNoteOff->deactivate();
		sync->deactivate();
	}

}







gTabBehaviors::gTabBehaviors(int X, int Y, int W, int H)
	: Fl_Group(X, Y, W, H, "Behaviors")
{
	begin();
	Fl_Group *radioGrp_1 = new Fl_Group(x(), y()+10, w(), 70); 
		new gBox(x(), y()+10, 70, 25, "When a channel with recorded actions is halted:", FL_ALIGN_LEFT);
		recsStopOnChanHalt_1 = new gRadio(x()+25, y()+35, 280, 20, "stop it immediately");
		recsStopOnChanHalt_0 = new gRadio(x()+25, y()+55, 280, 20, "play it until finished");
	radioGrp_1->end();

	Fl_Group *radioGrp_2 = new Fl_Group(x(), y()+70, w(), 70); 
		new gBox(x(), y()+80, 70, 25, "When the sequencer is halted:", FL_ALIGN_LEFT);
		chansStopOnSeqHalt_1 = new gRadio(x()+25, y()+105, 280, 20, "stop immediately all dynamic channels");
		chansStopOnSeqHalt_0 = new gRadio(x()+25, y()+125, 280, 20, "play all dynamic channels until finished");
	radioGrp_2->end();

	treatRecsAsLoops  = new gCheck(x(), y()+155, 280, 20, "Treat one shot channels with actions as loops");

	end();
	labelsize(GUI_FONT_SIZE_BASE);

	G_Conf.recsStopOnChanHalt == 1 ? recsStopOnChanHalt_1->value(1) : recsStopOnChanHalt_0->value(1);
	G_Conf.chansStopOnSeqHalt == 1 ? chansStopOnSeqHalt_1->value(1) : chansStopOnSeqHalt_0->value(1);
	G_Conf.treatRecsAsLoops   == 1 ? treatRecsAsLoops->value(1)  : treatRecsAsLoops->value(0);

	recsStopOnChanHalt_1->callback(cb_radio_mutex, (void*)this);
	recsStopOnChanHalt_0->callback(cb_radio_mutex, (void*)this);
	chansStopOnSeqHalt_1->callback(cb_radio_mutex, (void*)this);
	chansStopOnSeqHalt_0->callback(cb_radio_mutex, (void*)this);
}





void gTabBehaviors::cb_radio_mutex(Fl_Widget *w, void *p) { ((gTabBehaviors*)p)->__cb_radio_mutex(w); }





void gTabBehaviors::__cb_radio_mutex(Fl_Widget *w)
{
	((Fl_Button *)w)->type(FL_RADIO_BUTTON);
}





void gTabBehaviors::save()
{
	G_Conf.recsStopOnChanHalt = recsStopOnChanHalt_1->value() == 1 ? 1 : 0;
	G_Conf.chansStopOnSeqHalt = chansStopOnSeqHalt_1->value() == 1 ? 1 : 0;
	G_Conf.treatRecsAsLoops   = treatRecsAsLoops->value() == 1 ? 1 : 0;
}







#ifdef WITH_VST

gTabPlugins::gTabPlugins(int X, int Y, int W, int H)
	: Fl_Group(X, Y, W, H, "Plugins")
{
	folderPath = new gInput(x()+w()-250, y()+8, 250, 20);
	scanButton = new gClick(x()+w()-120, folderPath->y()+folderPath->h()+8, 120, 20);
	info       = new gBox(x(), scanButton->y()+scanButton->h()+8, w(), 242);

	end();

	labelsize(GUI_FONT_SIZE_BASE);

	info->label("Scan in progress. Please wait...");
	info->hide();

	folderPath->value(G_Conf.pluginPath.c_str());
	folderPath->label("Plugins folder");

	scanButton->callback(cb_scan, (void*) this);

	updateCount();
}





void gTabPlugins::updateCount()
{
	string scanLabel = "Scan (" + gItoa(G_PluginHost.countAvailablePlugins()) + " found)";
	scanButton->label(scanLabel.c_str());
}





void gTabPlugins::cb_scan(Fl_Widget *w, void *p) { ((gTabPlugins*)p)->__cb_scan(w); }





void gTabPlugins::cb_onScan()
{
	Fl::wait();
}





void gTabPlugins::__cb_scan(Fl_Widget *w)
{
	info->show();
	G_PluginHost.scanDir(folderPath->value(), cb_onScan);
	G_PluginHost.saveList(gGetHomePath() + gGetSlash() + "plugins.xml");
	info->hide();
	updateCount();
}





void gTabPlugins::save()
{
	G_Conf.pluginPath = folderPath->value();
}


#endif 







gdConfig::gdConfig(int w, int h) : gWindow(w, h, "Configuration")
{
	set_modal();

	if (G_Conf.configX)
		resize(G_Conf.configX, G_Conf.configY, this->w(), this->h());

	Fl_Tabs *tabs = new Fl_Tabs(8, 8, w-16, h-44);
		tabAudio     = new gTabAudio(tabs->x()+10, tabs->y()+20, tabs->w()-20, tabs->h()-40);
		tabMidi      = new gTabMidi(tabs->x()+10, tabs->y()+20, tabs->w()-20, tabs->h()-40);
		tabBehaviors = new gTabBehaviors(tabs->x()+10, tabs->y()+20, tabs->w()-20, tabs->h()-40);
		tabMisc      = new gTabMisc(tabs->x()+10, tabs->y()+20, tabs->w()-20, tabs->h()-40);
#ifdef WITH_VST
		tabPlugins   = new gTabPlugins(tabs->x()+10, tabs->y()+20, tabs->w()-20, tabs->h()-40);
#endif

	tabs->end();

	save 	 = new gClick (w-88, h-28, 80, 20, "Save");
	cancel = new gClick (w-176, h-28, 80, 20, "Cancel");

	end();

	tabs->box(FL_FLAT_BOX); 

	tabs->labelcolor(COLOR_TEXT_0);

	save->callback(cb_save_config, (void*)this);
	cancel->callback(cb_cancel, (void*)this);

	gu_setFavicon(this);
	setId(WID_CONFIG);
	show();
}





gdConfig::~gdConfig()
{
	G_Conf.configX = x();
	G_Conf.configY = y();
}





void gdConfig::cb_save_config(Fl_Widget *w, void *p) { ((gdConfig*)p)->__cb_save_config(); }
void gdConfig::cb_cancel     (Fl_Widget *w, void *p) { ((gdConfig*)p)->__cb_cancel(); }





void gdConfig::__cb_save_config()
{
	tabAudio->save();
	tabBehaviors->save();
	tabMidi->save();
	tabMisc->save();
#ifdef WITH_VST
	tabPlugins->save();
#endif
	do_callback();
}





void gdConfig::__cb_cancel()
{
	do_callback();
}
