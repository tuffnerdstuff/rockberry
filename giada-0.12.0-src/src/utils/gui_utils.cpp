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




#include "../core/mixer.h"
#include "../core/patch_DEPR_.h"
#include "../core/recorder.h"
#include "../core/wave.h"
#include "../core/pluginHost.h"
#include "../core/channel.h"
#include "../core/conf.h"
#include "../core/graphics.h"
#include "../gui/dialogs/gd_warnings.h"
#include "../gui/dialogs/gd_mainWindow.h"
#include "../gui/dialogs/gd_actionEditor.h"
#include "../gui/elems/ge_keyboard.h"
#include "../gui/elems/ge_window.h"
#include "../gui/elems/ge_channel.h"
#include "gui_utils.h"
#include "log.h"


extern Mixer 	       G_Mixer;
extern unsigned      G_beats;
extern bool 		     G_audio_status;
extern Patch_DEPR_   G_patch;
extern Conf          G_conf;
extern uint32_t      G_time;
extern gdMainWindow *mainWin;
#ifdef WITH_VST
extern PluginHost    G_PluginHost;
#endif


static int blinker = 0;


void gu_refresh()
{
	Fl::lock();

	

	mainWin->inOut->refresh();
	mainWin->beatMeter->redraw();
	mainWin->keyboard->refreshColumns();

	

	blinker++;
	if (blinker > 12)
		blinker = 0;

	

	Fl::unlock();
	Fl::awake();
}





int gu_getBlinker()
{
	return blinker;
}





void gu_updateControls()
{
	for (unsigned i=0; i<G_Mixer.channels.size(); i++) {
		G_Mixer.channels.at(i)->guiChannel->update();
	}

	mainWin->inOut->setOutVol(G_Mixer.outVol);
	mainWin->inOut->setInVol(G_Mixer.inVol);
#ifdef WITH_VST
	mainWin->inOut->setMasterFxOutFull(G_PluginHost.getStack(PluginHost::MASTER_OUT)->size() > 0);
	mainWin->inOut->setMasterFxInFull(G_PluginHost.getStack(PluginHost::MASTER_IN)->size() > 0);
#endif

	mainWin->timing->setMeter(G_Mixer.beats, G_Mixer.bars);
	mainWin->timing->setBpm(G_Mixer.bpm);

	

	mainWin->controller->updatePlay(G_Mixer.running);
	mainWin->controller->updateMetronome(0);
}





void gu_update_win_label(const char *c)
{
	std::string out = G_APP_NAME;
	out += " - ";
	out += c;
	mainWin->copy_label(out.c_str());
}





void gu_setFavicon(Fl_Window *w)
{
#if defined(__linux__)
	fl_open_display();
	Pixmap p, mask;
	XpmCreatePixmapFromData(
		fl_display,
		DefaultRootWindow(fl_display),
		(char **)giada_icon,
		&p,
		&mask,
		NULL);
	w->icon((char *)p);
#elif defined(_WIN32)
	w->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
#endif
}





void gu_openSubWindow(gWindow *parent, gWindow *child, int id)
{
	if (parent->hasWindow(id)) {
		gLog("[GU] parent has subwindow with id=%d, deleting\n", id);
		parent->delSubWindow(id);
	}
	child->setId(id);
	parent->addSubWindow(child);
}





void gu_refreshActionEditor()
{
	

	gdActionEditor *aeditor = (gdActionEditor*) mainWin->getChild(WID_ACTION_EDITOR);
	if (aeditor) {
		Channel *chan = aeditor->chan;
		mainWin->delSubWindow(WID_ACTION_EDITOR);
		gu_openSubWindow(mainWin, new gdActionEditor(chan), WID_ACTION_EDITOR);
	}
}





gWindow *gu_getSubwindow(gWindow *parent, int id)
{
	if (parent->hasWindow(id))
		return parent->getChild(id);
	else
		return NULL;
}





void gu_closeAllSubwindows()
{
	

	mainWin->delSubWindow(WID_ACTION_EDITOR);
	mainWin->delSubWindow(WID_SAMPLE_EDITOR);
	mainWin->delSubWindow(WID_FX_LIST);
	mainWin->delSubWindow(WID_FX);
}





string gu_removeFltkChars(const string &s)
{
	string out = gReplace(s, "/", "-");
	out = gReplace(out, "|", "-");
	out = gReplace(out, "&", "-");
	out = gReplace(out, "_", "-");
	return out;
}
