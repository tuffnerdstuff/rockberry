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




#include "../../core/pluginHost.h"
#include "../../core/mixer.h"
#include "../../core/conf.h"
#include "../../core/patch_DEPR_.h"
#include "../../core/graphics.h"
#include "../../core/channel.h"
#include "../../core/wave.h"
#include "../../core/sampleChannel.h"
#include "../../core/midiChannel.h"
#include "../../glue/glue.h"
#include "../../glue/channel.h"
#include "../../utils/gui_utils.h"
#include "../dialogs/gd_mainWindow.h"
#include "../dialogs/gd_keyGrabber.h"
#include "../dialogs/gd_midiInput.h"
#include "../dialogs/gd_editor.h"
#include "../dialogs/gd_actionEditor.h"
#include "../dialogs/gd_warnings.h"
#include "../dialogs/gd_browser.h"
#include "../dialogs/gd_midiOutput.h"
#include "../dialogs/gd_pluginList.h"
#include "../dialogs/gd_pluginChooser.h"
#include "ge_keyboard.h"
#include "ge_sampleChannel.h"
#include "ge_status.h"
#include "ge_modeBox.h"


extern Mixer 		     G_Mixer;
extern Conf  		     G_Conf;
extern Patch_DEPR_   G_Patch_DEPR_;
extern gdMainWindow *mainWin;


gSampleChannel::gSampleChannel(int X, int Y, int W, int H, class SampleChannel *ch)
	: gChannel(X, Y, W, H, CHANNEL_SAMPLE), ch(ch)
{
	begin();

#if defined(WITH_VST)
  int delta = 168; 
#else
	int delta = 144; 
#endif

	button       = new gButton(x(), y(), 20, 20, "", channelStop_xpm, channelPlay_xpm);
	status       = new gStatus(button->x()+button->w()+4, y(), 20, 20, ch);
	mainButton   = new gSampleChannelButton(status->x()+status->w()+4, y(), w() - delta, 20, "-- no sample --");
	modeBox      = new gModeBox(mainButton->x()+mainButton->w()+4, y(), 20, 20, ch);
	mute         = new gClick(modeBox->x()+modeBox->w()+4, y(), 20, 20, "", muteOff_xpm, muteOn_xpm);
	solo         = new gClick(mute->x()+mute->w()+4, y(), 20, 20, "", soloOff_xpm, soloOn_xpm);
	readActions  = NULL; 

#if defined(WITH_VST)
	fx           = new gFxButton(solo->x()+solo->w()+4, y(), 20, 20, fxOff_xpm, fxOn_xpm);
	vol          = new gDial(fx->x()+fx->w()+4, y(), 20, 20);
#else
	vol          = new gDial(solo->x()+solo->w()+4, y(), 20, 20);
#endif

	end();

  resizable(mainButton);

	update();

	button->callback(cb_button, (void*)this);
	button->when(FL_WHEN_CHANGED);   

#ifdef WITH_VST
	fx->callback(cb_openFxWindow, (void*)this);
#endif

	mute->type(FL_TOGGLE_BUTTON);
	mute->callback(cb_mute, (void*)this);

	solo->type(FL_TOGGLE_BUTTON);
	solo->callback(cb_solo, (void*)this);

	mainButton->callback(cb_openMenu, (void*)this);

	vol->callback(cb_changeVol, (void*)this);

	ch->guiChannel = this;
}





void gSampleChannel::cb_button      (Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_button(); }
void gSampleChannel::cb_mute        (Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_mute(); }
void gSampleChannel::cb_solo        (Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_solo(); }
void gSampleChannel::cb_openMenu    (Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_openMenu(); }
void gSampleChannel::cb_changeVol   (Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_changeVol(); }
void gSampleChannel::cb_readActions (Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_readActions(); }
#ifdef WITH_VST
void gSampleChannel::cb_openFxWindow(Fl_Widget *v, void *p) { ((gSampleChannel*)p)->__cb_openFxWindow(); }
#endif





void gSampleChannel::__cb_mute()
{
	glue_setMute(ch);
}





void gSampleChannel::__cb_solo()
{
	solo->value() ? glue_setSoloOn(ch) : glue_setSoloOff(ch);
}





void gSampleChannel::__cb_changeVol()
{
	glue_setChanVol(ch, vol->value());
}





#ifdef WITH_VST
void gSampleChannel::__cb_openFxWindow()
{
	gu_openSubWindow(mainWin, new gdPluginList(PluginHost::CHANNEL, ch), WID_FX_LIST);
}
#endif






void gSampleChannel::__cb_button()
{
	if (button->value())    
		glue_keyPress(ch, Fl::event_ctrl(), Fl::event_shift());
	else                    
		glue_keyRelease(ch, Fl::event_ctrl(), Fl::event_shift());
}





void gSampleChannel::__cb_openMenu()
{
	

	if (G_Mixer.chanInput == ch || recorder::active)
		return;

	

	Fl_Menu_Item rclick_menu[] = {
		{"Load new sample..."},                     
		{"Export sample to file..."},               
		{"Setup keyboard input..."},                
		{"Setup MIDI input..."},                    
		{"Setup MIDI output..."},                   
		{"Edit sample..."},                         
		{"Edit actions..."},                        
		{"Clear actions", 0, 0, 0, FL_SUBMENU},     
			{"All"},                                  
			{"Mute"},                                 
			{"Volume"},                               
			{"Start/Stop"},                           
			{0},                                      
			{"Clone channel"},                        
		{"Free channel"},                           
		{"Delete channel"},                         
		{0}
	};

	if (ch->status & (STATUS_EMPTY | STATUS_MISSING)) {
		rclick_menu[1].deactivate();
		rclick_menu[5].deactivate();
		rclick_menu[14].deactivate();
	}

	

	if (!ch->hasActions)
		rclick_menu[7].deactivate();

	

	if (ch->mode & LOOP_ANY)
		rclick_menu[11].deactivate();

	Fl_Menu_Button *b = new Fl_Menu_Button(0, 0, 100, 50);
	b->box(G_BOX);
	b->textsize(GUI_FONT_SIZE_BASE);
	b->textcolor(COLOR_TEXT_0);
	b->color(COLOR_BG_0);

	const Fl_Menu_Item *m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, b);
	if (!m) return;

	if (strcmp(m->label(), "Load new sample...") == 0) {
		openBrowser(BROWSER_LOAD_SAMPLE);
		return;
	}

	if (strcmp(m->label(), "Setup keyboard input...") == 0) {
		new gdKeyGrabber(ch); 
		return;
	}

	if (strcmp(m->label(), "Setup MIDI input...") == 0) {
		gu_openSubWindow(mainWin, new gdMidiInputChannel(ch), 0);
		return;
	}

	if (strcmp(m->label(), "Setup MIDI output...") == 0) {
		gu_openSubWindow(mainWin, new gdMidiOutputSampleCh(ch), 0);
		return;
	}

	if (strcmp(m->label(), "Edit sample...") == 0) {
		gu_openSubWindow(mainWin, new gdEditor(ch), WID_SAMPLE_EDITOR); 
		return;
	}

	if (strcmp(m->label(), "Export sample to file...") == 0) {
		openBrowser(BROWSER_SAVE_SAMPLE);
		return;
	}

	if (strcmp(m->label(), "Delete channel") == 0) {
		if (!gdConfirmWin("Warning", "Delete channel: are you sure?"))
			return;
		glue_deleteChannel(ch);
		return;
	}

	if (strcmp(m->label(), "Free channel") == 0) {
		if (ch->status == STATUS_PLAY) {
			if (!gdConfirmWin("Warning", "This action will stop the channel: are you sure?"))
				return;
		}
		else if (!gdConfirmWin("Warning", "Free channel: are you sure?"))
			return;

		glue_freeChannel(ch);

		

		

		mainWin->delSubWindow(WID_FILE_BROWSER);
		mainWin->delSubWindow(WID_ACTION_EDITOR);
		mainWin->delSubWindow(WID_SAMPLE_EDITOR);
		mainWin->delSubWindow(WID_FX_LIST);

		return;
	}

	if (strcmp(m->label(), "Clone channel") == 0) {
		glue_cloneChannel(ch);
		return;
	}

	if (strcmp(m->label(), "Mute") == 0) {
		if (!gdConfirmWin("Warning", "Clear all mute actions: are you sure?"))
			return;
		recorder::clearAction(ch->index, ACTION_MUTEON | ACTION_MUTEOFF);
		if (!ch->hasActions)
			delActionButton();

		

		gu_refreshActionEditor(); 
		return;
	}

	if (strcmp(m->label(), "Start/Stop") == 0) {
		if (!gdConfirmWin("Warning", "Clear all start/stop actions: are you sure?"))
			return;
		recorder::clearAction(ch->index, ACTION_KEYPRESS | ACTION_KEYREL | ACTION_KILLCHAN);
		if (!ch->hasActions)
			delActionButton();
		gu_refreshActionEditor();  
		return;
	}

	if (strcmp(m->label(), "Volume") == 0) {
		if (!gdConfirmWin("Warning", "Clear all volume actions: are you sure?"))
			return;
		recorder::clearAction(ch->index, ACTION_VOLUME);
		if (!ch->hasActions)
			delActionButton();
		gu_refreshActionEditor();  
		return;
	}

	if (strcmp(m->label(), "All") == 0) {
		if (!gdConfirmWin("Warning", "Clear all actions: are you sure?"))
			return;
		recorder::clearChan(ch->index);
		delActionButton();
		gu_refreshActionEditor(); 
		return;
	}

	if (strcmp(m->label(), "Edit actions...") == 0) {
		gu_openSubWindow(mainWin, new gdActionEditor(ch),	WID_ACTION_EDITOR);
		return;
	}
}





void gSampleChannel::__cb_readActions()
{
	ch->readActions ? glue_stopReadingRecs(ch) : glue_startReadingRecs(ch);
}





void gSampleChannel::openBrowser(int type)
{
	const char *title = "";
	switch (type) {
		case BROWSER_LOAD_SAMPLE:
			title = "Browse Sample";
			break;
		case BROWSER_SAVE_SAMPLE:
			title = "Save Sample";
			break;
		case -1:
			title = "Edit Sample";
			break;
	}
	gWindow *childWin = new gdBrowser(title, G_Conf.samplePath.c_str(), ch, type);
	gu_openSubWindow(mainWin, childWin,	WID_FILE_BROWSER);
}





void gSampleChannel::refresh()
{
  if (!mainButton->visible()) 
    return;

	setColorsByStatus(ch->status, ch->recStatus);

	if (ch->wave != NULL) {
		if (G_Mixer.chanInput == ch)
			mainButton->setInputRecordMode();
		if (recorder::active) {
			if (recorder::canRec(ch))
				mainButton->setActionRecordMode();
		}
		status->redraw(); 
	}
	mainButton->redraw();
}





void gSampleChannel::reset()
{
	delActionButton(true); 
	mainButton->setDefaultMode("-- no sample --");
 	mainButton->redraw();
	status->redraw();
}





void gSampleChannel::update()
{
	

	switch (ch->status) {
		case STATUS_EMPTY:
			mainButton->label("-- no sample --");
			break;
		case STATUS_MISSING:
		case STATUS_WRONG:
			mainButton->label("* file not found! *");
			break;
		default:
			mainButton->label(ch->wave->name.c_str());
			break;
	}

	

	if (ch->hasActions)
		addActionButton();
	else
		delActionButton();

	

	modeBox->value(ch->mode);
	modeBox->redraw();

	

	vol->value(ch->volume);
	mute->value(ch->mute);
	solo->value(ch->solo);

	mainButton->setKey(ch->key);

#ifdef WITH_VST
	fx->full = ch->plugins_DEPR_.size() > 0;
	fx->redraw();
#endif
}





int gSampleChannel::keyPress(int e)
{
	return handleKey(e, ch->key);
}





void gSampleChannel::addActionButton()
{
	

	if (readActions != NULL)
		return;

	mainButton->size(mainButton->w()-24, mainButton->h());

	redraw();

	readActions = new gClick(mainButton->x() + mainButton->w() + 4,
                           mainButton->y(), 20, 20, "", readActionOff_xpm,
                           readActionOn_xpm);
	readActions->type(FL_TOGGLE_BUTTON);
	readActions->value(ch->readActions);
	readActions->callback(cb_readActions, (void*)this);
	add(readActions);

	

	mainWin->keyboard->redraw();
}





void gSampleChannel::delActionButton(bool force)
{
	if (readActions == NULL)
		return;

	

	if (!force && (readActions == NULL || ch->hasActions))
		return;

	remove(readActions);		
	delete readActions;     
	readActions = NULL;

	mainButton->size(mainButton->w()+24, mainButton->h());
	mainButton->redraw();
}





void gSampleChannel::resize(int X, int Y, int W, int H)
{
  gChannel::resize(X, Y, W, H);

	if (w() < BREAK_FX) {
#ifdef WITH_VST
		fx->hide();
#endif
		mainButton->size(w() - BREAK_DELTA, mainButton->h());
		mute->resize(mainButton->x()+mainButton->w()+4, y(), 20, 20);
		solo->resize(mute->x()+mute->w()+4, y(), 20, 20);

		

	  modeBox->hide();
		if (readActions)
      readActions->hide();
	}
	else
	if (w() < BREAK_MODE_BOX) {
#ifdef WITH_VST
		fx->show();
#endif
		mainButton->size(w() - (BREAK_DELTA + BREAK_UNIT), mainButton->h());
		mute->resize(mainButton->x()+mainButton->w()+4, y(), 20, 20);
		solo->resize(mute->x()+mute->w()+4, y(), 20, 20);
    modeBox->hide();
	}
	else
	if (w() < BREAK_READ_ACTIONS) {
    modeBox->show();
    mainButton->size(w() - (BREAK_DELTA + (BREAK_UNIT * 2)), mainButton->h());
    modeBox->resize(mainButton->x()+mainButton->w()+4, y(), 20, 20);
		if (readActions)
      readActions->hide();
	}
	else {
		if (readActions) {
      mainButton->size(w() - (BREAK_DELTA + (BREAK_UNIT * 3)), mainButton->h());
      readActions->resize(mainButton->x()+mainButton->w()+4, y(), 20, 20);
      readActions->show();
		}
	}

	gChannel::init_sizes();
}







gSampleChannelButton::gSampleChannelButton(int x, int y, int w, int h, const char *l)
	: gChannelButton(x, y, w, h, l) {}





int gSampleChannelButton::handle(int e)
{
	int ret = gClick::handle(e);
	switch (e) {
		case FL_DND_ENTER:
		case FL_DND_DRAG:
		case FL_DND_RELEASE: {
			ret = 1;
			break;
		}
		case FL_PASTE: {
      gSampleChannel *gch = (gSampleChannel*) parent();   
      SampleChannel  *ch  = gch->ch;
      int result = glue_loadChannel(ch, gTrim(gStripFileUrl(Fl::event_text())).c_str());
			if (result != SAMPLE_LOADED_OK)
				mainWin->keyboard->printChannelMessage(result);
			ret = 1;
			break;
		}
	}
	return ret;
}
