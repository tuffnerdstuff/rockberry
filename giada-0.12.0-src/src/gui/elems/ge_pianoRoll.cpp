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




#include <FL/fl_draw.H>
#include "../../core/channel.h"
#include "../../core/midiChannel.h"
#include "../../core/const.h"
#include "../../core/kernelMidi.h"
#include "../../core/conf.h"
#include "../../utils/log.h"
#include "../dialogs/gd_mainWindow.h"
#include "../dialogs/gd_actionEditor.h"
#include "ge_pianoRoll.h"


extern gdMainWindow *mainWin;
extern Mixer         G_Mixer;
extern Conf	         G_Conf;


gPianoRollContainer::gPianoRollContainer(int x, int y, class gdActionEditor *pParent)
 : Fl_Scroll(x, y, 200, 422), pParent(pParent)
{
	size(pParent->totalWidth, G_Conf.pianoRollH);
	pianoRoll = new gPianoRoll(x, y, pParent->totalWidth, pParent);
}





gPianoRollContainer::~gPianoRollContainer() {
	clear();
	G_Conf.pianoRollH = h();
	G_Conf.pianoRollY = pianoRoll->y();
}





void gPianoRollContainer::updateActions() {
	pianoRoll->updateActions();
}





void gPianoRollContainer::draw() {

	pianoRoll->size(this->w(), pianoRoll->h());  

	

	fl_rectf(x(), y(), w(), h(), COLOR_BG_MAIN);

	

	fl_push_clip(x(), y(), w(), h());
	draw_child(*pianoRoll);
	fl_pop_clip();

	fl_color(COLOR_BD_0);
	fl_line_style(0);
	fl_rect(x(), y(), pParent->totalWidth, h());
}







gPianoRoll::gPianoRoll(int X, int Y, int W, class gdActionEditor *pParent)
 : gActionWidget(X, Y, W, 40, pParent)
{
	resizable(NULL);                      
	size(W, (MAX_NOTES+1) * CELL_H);      

	if (G_Conf.pianoRollY == -1)
		position(x(), y()-(h()/2));  
	else
		position(x(), G_Conf.pianoRollY);

	drawSurface1();
	drawSurface2();

	

	recorder::sortActions();

	recorder::action *a2   = NULL;
	recorder::action *prev = NULL;

	for (unsigned i=0; i<recorder::frames.size(); i++) {
		for (unsigned j=0; j<recorder::global.at(i).size(); j++) {

			
			

			if (recorder::frames.at(i) > G_Mixer.totalFrames)
				continue;

			recorder::action *a1 = recorder::global.at(i).at(j);

			if (a1->chan != pParent->chan->index)
				continue;

			if (a1->type == ACTION_MIDI) {

				

				if (a1 == prev) {
					
					continue;
				}

				

				int a1_type = kernelMidi::getB1(a1->iValue);
				int a1_note = kernelMidi::getB2(a1->iValue);
				int a1_velo = kernelMidi::getB3(a1->iValue);

				if (a1_type == 0x80) {
					
					continue;
				}

				

				recorder::getNextAction(
						a1->chan,
						ACTION_MIDI,
						a1->frame,
						&a2,
						kernelMidi::getIValue(0x80, a1_note, a1_velo));

				

				if (a2) {
					
					
					
					new gPianoItem(0, 0, x(), y()+3, a1, a2, pParent);
					prev = a2;
					a2 = NULL;
				}
				else
					gLog("[gPianoRoll] recorder didn't find action!\n");

			}
		}
	}

	end();
}





void gPianoRoll::drawSurface1() {

	surface1 = fl_create_offscreen(40, h());
	fl_begin_offscreen(surface1);

	

	fl_rectf(0, 0, 40, h(), COLOR_BG_MAIN);

	fl_line_style(FL_DASH, 0, NULL);
	fl_font(FL_HELVETICA, 11);

	int octave = 9;

	for (int i=1; i<=MAX_NOTES+1; i++) {

		

		char note[6];
		int  step = i % 12;

		switch (step) {
			case 1:
				fl_rectf(0, i*CELL_H, 40, CELL_H, 30, 30, 30);
				sprintf(note, "%dG", octave);
				break;
			case 2:
				sprintf(note, "%dF#", octave);
				break;
			case 3:
				sprintf(note, "%dF", octave);
				break;
			case 4:
				fl_rectf(0, i*CELL_H, 40, CELL_H, 30, 30, 30);
				sprintf(note, "%dE", octave);
				break;
			case 5:
				sprintf(note, "%dD#", octave);
				break;
			case 6:
				fl_rectf(0, i*CELL_H, 40, CELL_H, 30, 30, 30);
				sprintf(note, "%dD", octave);
				break;
			case 7:
				sprintf(note, "%dC#", octave);
				break;
			case 8:
				sprintf(note, "%dC", octave);
				break;
			case 9:
				fl_rectf(0, i*CELL_H, 40, CELL_H, 30, 30, 30);
				sprintf(note, "%dB", octave);
				break;
			case 10:
				sprintf(note, "%dA#", octave);
				break;
			case 11:
				fl_rectf(0, i*CELL_H, 40, CELL_H, 30, 30, 30);
				sprintf(note, "%dA", octave);
				break;
			case 0:
				sprintf(note, "%dG#", octave);
				octave--;
				break;
		}

		fl_color(fl_rgb_color(54, 54, 54));
		fl_draw(note, 4, ((i-1)*CELL_H)+1, 30, CELL_H, (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_CENTER));

		

		if (i < 128)
			fl_line(0, i*CELL_H, 40, +i*CELL_H);
	}

	fl_line_style(0);
	fl_end_offscreen();
}





void gPianoRoll::drawSurface2() {
	surface2 = fl_create_offscreen(40, h());
	fl_begin_offscreen(surface2);
	fl_rectf(0, 0, 40, h(), COLOR_BG_MAIN);
	fl_color(fl_rgb_color(54, 54, 54));
	fl_line_style(FL_DASH, 0, NULL);
	for (int i=1; i<=MAX_NOTES+1; i++) {
		int  step = i % 12;
		switch (step) {
			case 1:
			case 4:
			case 6:
			case 9:
			case 11:
				fl_rectf(0, i*CELL_H, 40, CELL_H, 30, 30, 30);
				break;
		}
		if (i < 128) {
			fl_color(fl_rgb_color(54, 54, 54));
			fl_line(0, i*CELL_H, 40, +i*CELL_H);
		}
	}
	fl_line_style(0);
	fl_end_offscreen();
}





void gPianoRoll::draw() {

	fl_copy_offscreen(x(), y(), 40, h(), surface1, 0, 0);

#if defined(__APPLE__)
	for (int i=36; i<pParent->totalWidth; i+=36) 
		fl_copy_offscreen(x()+i, y(), 40, h(), surface2, 1, 0);
#else
	for (int i=40; i<pParent->totalWidth; i+=40) 
		fl_copy_offscreen(x()+i, y(), 40, h(), surface2, 0, 0);
#endif

	baseDraw(false);
	draw_children();
}





int gPianoRoll::handle(int e) {

	int ret = Fl_Group::handle(e);

	switch (e) {
		case FL_PUSH:	{

			

			if (Fl::event_x() >= pParent->coverX) {
				ret = 1;
				break;
			}


			push_y = Fl::event_y() - y();

			if (Fl::event_button1()) {

				

				int ax = Fl::event_x();
				int ay = Fl::event_y();

				

				int edge = (ay-y()-3) % 15;
				if (edge != 0) ay -= edge;

				

				if (!onItem(ax, ay-y()-3)) {
					int greyover = ax+20 - pParent->coverX-x();
					if (greyover > 0)
						ax -= greyover;
					add(new gPianoItem(ax, ay, ax-x(), ay-y()-3, NULL, NULL, pParent));
					redraw();
				}
			}
			ret = 1;
			break;
		}
		case FL_DRAG:	{

			if (Fl::event_button3()) {

				gPianoRollContainer *prc = (gPianoRollContainer*) parent();
				position(x(), Fl::event_y() - push_y);

				if (y() > prc->y())
					position(x(), prc->y());
				else
				if (y() < prc->y()+prc->h()-h())
					position(x(), prc->y()+prc->h()-h());

				prc->redraw();
			}
			ret = 1;
			break;
		}
		case FL_MOUSEWHEEL: {   
			ret = 1;
			break;
		}
	}
	return ret;
}





void gPianoRoll::updateActions() {

	

	gPianoItem *i;
	for (int k=0; k<children(); k++) {
		i = (gPianoItem*) child(k);

		

		int newX = x() + (i->getFrame_a() / pParent->zoom);
		int newW = ((i->getFrame_b() - i->getFrame_a()) / pParent->zoom);
		if (newW < 8)
			newW = 8;
		i->resize(newX, i->y(), newW, i->h());
		i->redraw();

		
	}
}





bool gPianoRoll::onItem(int rel_x, int rel_y) {

	if (!pParent->chan->hasActions)
		return false;

	int note = MAX_NOTES - (rel_y / CELL_H);

	int n = children();
	for (int i=0; i<n; i++) {   

		gPianoItem *p = (gPianoItem*) child(i);
		if (p->getNote() != note)
			continue;

		

		int start = p->x() > rel_x ? p->x() : rel_x-1;
		int end   = p->x()+p->w() < rel_x + 20 ? p->x()+p->w() : rel_x + 21;
		if (start < end)
			return true;
	}
	return false;
}






gPianoItem::gPianoItem(int X, int Y, int rel_x, int rel_y, recorder::action *_a, recorder::action *_b, gdActionEditor *pParent)
	: Fl_Box  (X, Y, 20, gPianoRoll::CELL_H-5),
	  a       (_a),
	  b       (_b),
		pParent (pParent),
		selected(false),
		event_a (0x00),
		event_b (0x00),
		changed (false)
{

	

	if (a) {
		note    = kernelMidi::getB2(a->iValue);
		frame_a = a->frame;
		frame_b = b->frame;
		event_a = a->iValue;
		event_b = b->iValue;
		int newX = rel_x + (frame_a / pParent->zoom);
		int newY = rel_y + getY(note);
		int newW = (frame_b - frame_a) / pParent->zoom;
		resize(newX, newY, newW, h());
	}

	

	else {
		note    = getNote(rel_y);
		frame_a = rel_x * pParent->zoom;
		frame_b = (rel_x + 20) * pParent->zoom;
		record();
		size((frame_b - frame_a) / pParent->zoom, h());
	}
}





bool gPianoItem::overlap() {

	

	gPianoRoll *pPiano = (gPianoRoll*) parent();

	for (int i=0; i<pPiano->children(); i++) {

		gPianoItem *pItem = (gPianoItem*) pPiano->child(i);

		

		if (pItem == this || pItem->y() != y())
			continue;

		int start = pItem->x() >= x() ? pItem->x() : x();
		int end   = pItem->x()+pItem->w() < x()+w() ? pItem->x()+pItem->w() : x()+w();
		if (start < end)
			return true;
	}

	return false;
}





void gPianoItem::draw() {
	int _w = w() > 4 ? w() : 4;
	
	fl_rectf(x(), y(), _w, h(), (Fl_Color) selected ? COLOR_BD_1 : COLOR_BG_2);
}





void gPianoItem::record() {

	

	int overflow = frame_b - G_Mixer.totalFrames;
	if (overflow > 0) {
		frame_b -= overflow;
		frame_a -= overflow;
	}

	
	
	event_a |= (0x90 << 24);   
	event_a |= (note << 16);   
	event_a |= (0x3F <<  8);   
	event_a |= (0x00);

	event_b |= (0x80 << 24);   
	event_b |= (note << 16);   
	event_b |= (0x3F <<  8);   
	event_b |= (0x00);

	recorder::rec(pParent->chan->index, ACTION_MIDI, frame_a, event_a);
	recorder::rec(pParent->chan->index, ACTION_MIDI, frame_b, event_b);
}





void gPianoItem::remove() {
	recorder::deleteAction(pParent->chan->index, frame_a, ACTION_MIDI, true, event_a, 0.0);
	recorder::deleteAction(pParent->chan->index, frame_b, ACTION_MIDI, true, event_b, 0.0);

	

	((MidiChannel*) pParent->chan)->sendMidi(event_b);
}





int gPianoItem::handle(int e) {

	int ret = 0;

	switch (e) {

		case FL_ENTER: {
			selected = true;
			ret = 1;
			redraw();
			break;
		}

		case FL_LEAVE: {
			fl_cursor(FL_CURSOR_DEFAULT, FL_WHITE, FL_BLACK);
			selected = false;
			ret = 1;
			redraw();
			break;
		}

		case FL_MOVE: {
			onLeftEdge  = false;
			onRightEdge = false;

			if (Fl::event_x() >= x() && Fl::event_x() < x()+4) {
				onLeftEdge = true;
				fl_cursor(FL_CURSOR_WE, FL_WHITE, FL_BLACK);
			}
			else
			if (Fl::event_x() >= x()+w()-4 && Fl::event_x() <= x()+w()) {
				onRightEdge = true;
				fl_cursor(FL_CURSOR_WE, FL_WHITE, FL_BLACK);
			}
			else
				fl_cursor(FL_CURSOR_DEFAULT, FL_WHITE, FL_BLACK);

			ret = 1;
			break;
		}

		case FL_PUSH: {

			push_x = Fl::event_x() - x();
			old_x  = x();
			old_w  = w();

			if (Fl::event_button3()) {
				fl_cursor(FL_CURSOR_DEFAULT, FL_WHITE, FL_BLACK);
				remove();
				hide();   
				Fl::delete_widget(this);
				((gPianoRoll*)parent())->redraw();
			}
			ret = 1;
			break;
		}

		case FL_DRAG: {

			changed = true;

			gPianoRoll *pr = (gPianoRoll*) parent();
			int coverX     = pParent->coverX + pr->x(); 
			int nx, ny, nw;

			if (onLeftEdge) {
				nx = Fl::event_x();
				ny = y();
				nw = x()-Fl::event_x()+w();
				if (nx < pr->x()) {
					nx = pr->x();
					nw = w()+x()-pr->x();
				}
				else
				if (nx > x()+w()-8) {
					nx = x()+w()-8;
					nw = 8;
				}
				resize(nx, ny, nw, h());
			}
			else
			if (onRightEdge) {
				nw = Fl::event_x()-x();
				if (Fl::event_x() < x()+8)
					nw = 8;
				else
				if (Fl::event_x() > coverX)
					nw = coverX-x();
				size(nw, h());
			}
			else {
				nx = Fl::event_x() - push_x;
				if (nx < pr->x()+1)
					nx = pr->x()+1;
				else
				if (nx+w() > coverX)
					nx = coverX-w();

				

				if (pParent->gridTool->isOn())
					nx = pParent->gridTool->getSnapPoint(nx-pr->x()) + pr->x() - 1;

				position(nx, y());
			}

			

			redraw();
			((gPianoRoll*)parent())->redraw();
			ret = 1;
			break;
		}

		case FL_RELEASE: {

			

			if (overlap()) {
				resize(old_x, y(), old_w, h());
				redraw();
			}
			else
			if (changed) {
				remove();
				note    = getNote(getRelY());
				frame_a = getRelX() * pParent->zoom;
				frame_b = (getRelX()+w()) * pParent->zoom;
				record();
				changed = false;
			}

			((gPianoRoll*)parent())->redraw();

			ret = 1;
			break;
		}
	}
	return ret;
}
