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




#include "../../core/mixer.h"
#include "../../core/conf.h"
#include "../../core/patch_DEPR_.h"
#include "../../core/channel.h"
#include "../../core/sampleChannel.h"
#include "../../core/midiChannel.h"
#include "../../glue/glue.h"
#include "../../glue/channel.h"
#include "../../utils/log.h"
#include "../dialogs/gd_mainWindow.h"
#include "../dialogs/gd_warnings.h"
#include "../elems/ge_keyboard.h"
#include "ge_column.h"
#include "ge_channel.h"
#include "ge_sampleChannel.h"
#include "ge_midiChannel.h"


extern Mixer 		     G_Mixer;
extern Conf  		     G_Conf;
extern Patch_DEPR_   G_Patch_DEPR_;
extern gdMainWindow *mainWin;


gColumn::gColumn(int X, int Y, int W, int H, int index, gKeyboard *parent)
	: Fl_Group(X, Y, W, H), parent(parent), index(index)
{
  

	begin();
	addChannelBtn = new gClick(x(), y(), w(), 20, "Add new channel");
	end();

  resizer = new gResizerBar(x()+w(), y(), 16, h(), false);
  resizer->setMinSize(MIN_COLUMN_WIDTH);
  parent->add(resizer);

	addChannelBtn->callback(cb_addChannel, (void*)this);
}





gColumn::~gColumn()
{
  

  parent->remove(resizer);
}






int gColumn::handle(int e)
{
	switch (e) {
		case FL_RELEASE: {
			if (Fl::event_button() == FL_RIGHT_MOUSE) {
				__cb_addChannel();
				return 1;
			}
		}
		case FL_DND_ENTER:           	
		case FL_DND_DRAG:
		case FL_DND_RELEASE: {
			return 1;
		}
		case FL_PASTE: {              
			vector<std::string> paths;
			gSplit(Fl::event_text(), "\n", &paths);
			bool fails = false;
			int result = 0;
			for (unsigned i=0; i<paths.size(); i++) {
				gLog("[gColumn::handle] loading %s...\n", paths.at(i).c_str());
				SampleChannel *c = (SampleChannel*) glue_addChannel(index, CHANNEL_SAMPLE);
				result = glue_loadChannel(c, gStripFileUrl(paths.at(i).c_str()).c_str());
				if (result != SAMPLE_LOADED_OK) {
					deleteChannel(c->guiChannel);
					fails = true;
				}
			}
			if (fails) {
				if (paths.size() > 1)
					gdAlert("Some files were not loaded successfully.");
				else
					parent->printChannelMessage(result);
			}
			return 1;
		}
	}

	

	return Fl_Group::handle(e);
}





void gColumn::resize(int X, int Y, int W, int H)
{
  

  int ch = children();
  for (int i=0; i<ch; i++) {
    Fl_Widget *c = child(i);
    c->resize(X, Y + (i * (c->h() + 4)), W, c->h());
  }

  

  x(X); y(Y); w(W); h(H);

  

  resizer->size(16, H);
}





void gColumn::refreshChannels()
{
	for (int i=1; i<children(); i++)
		((gChannel*) child(i))->refresh();
}





void gColumn::draw()
{
	fl_color(fl_rgb_color(27, 27, 27));
	fl_rectf(x(), y(), w(), h());

  

  for (int i=0; i<children(); i++) {
    child(i)->draw();
    child(i)->redraw();
  }
}





void gColumn::cb_addChannel(Fl_Widget *v, void *p) { ((gColumn*)p)->__cb_addChannel(); }





gChannel *gColumn::addChannel(class Channel *ch)
{
	int currentY = y() + children() * 24;
	gChannel *gch = NULL;
	if (ch->type == CHANNEL_SAMPLE)
		gch = (gSampleChannel*) new gSampleChannel(x(), currentY, w(), 20, (SampleChannel*) ch);
	else
		gch = (gMidiChannel*) new gMidiChannel(x(),	currentY, w(), 20, (MidiChannel*) ch);

	add(gch);
  resize(x(), y(), w(), (children() * 24) + 66); 
  gch->redraw();    
	parent->redraw(); 
	return gch;
}





void gColumn::deleteChannel(gChannel *gch)
{
	gch->hide();
	remove(gch);
	delete gch;

	
	

	for (int i=0; i<children(); i++) {
		gch = (gChannel*) child(i);
		gch->position(gch->x(), y()+(i*24));
	}
	size(w(), children() * 24 + 66);  
	redraw();
}





void gColumn::__cb_addChannel()
{
	gLog("[gColumn::__cb_addChannel] index = %d\n", index);
	int type = openTypeMenu();
	if (type)
		glue_addChannel(index, type);
}





int gColumn::openTypeMenu()
{
	Fl_Menu_Item rclick_menu[] = {
		{"Sample channel"},
		{"MIDI channel"},
		{0}
	};

	Fl_Menu_Button *b = new Fl_Menu_Button(0, 0, 100, 50);
	b->box(G_BOX);
	b->textsize(GUI_FONT_SIZE_BASE);
	b->textcolor(COLOR_TEXT_0);
	b->color(COLOR_BG_0);

	const Fl_Menu_Item *m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, b);
	if (!m) return 0;

	if (strcmp(m->label(), "Sample channel") == 0)
		return CHANNEL_SAMPLE;
	if (strcmp(m->label(), "MIDI channel") == 0)
		return CHANNEL_MIDI;
	return 0;
}





void gColumn::clear(bool full)
{
	if (full)
		Fl_Group::clear();
	else {
		while (children() >= 2) {  
			int i = children()-1;
			deleteChannel((gChannel*)child(i));
		}
	}
}





Channel *gColumn::getChannel(int i)
{
  gChannel *gch = (gChannel*) child(i);
  if (gch->type == CHANNEL_SAMPLE)
    return ((gSampleChannel*) child(i))->ch;
  else
    return ((gMidiChannel*) child(i))->ch;
}
