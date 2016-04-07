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
#include "../../core/conf.h"
#include "../../core/channel.h"
#include "../../core/sampleChannel.h"
#include "../../glue/glue.h"
#include "../../utils/log.h"
#include "../dialogs/gd_mainWindow.h"
#include "../dialogs/gd_actionEditor.h"
#include "ge_keyboard.h"
#include "ge_actionChannel.h"


extern gdMainWindow *mainWin;
extern Mixer         G_Mixer;
extern Conf	         G_Conf;





gActionChannel::gActionChannel(int x, int y, gdActionEditor *pParent, SampleChannel *ch)
 : gActionWidget(x, y, 200, 40, pParent), ch(ch), selected(NULL)
{
	size(pParent->totalWidth, h());

	

	for (unsigned i=0; i<recorder::frames.size(); i++) {
		for (unsigned j=0; j<recorder::global.at(i).size(); j++) {

			recorder::action *ra = recorder::global.at(i).at(j);

			if (ra->chan == pParent->chan->index) {

				

				if (recorder::frames.at(i) > G_Mixer.totalFrames)
					continue;

				

				if (ra->type == ACTION_KILLCHAN && ch->mode == SINGLE_PRESS)
					continue;

				

				if (ra->type & (ACTION_KEYPRESS | ACTION_KILLCHAN))	{
					int ax = x+(ra->frame/pParent->zoom);
					gAction *a = new gAction(
							ax,           
							y+4,          
							h()-8,        
							ra->frame,	  
							i,            
							pParent,      
							ch,           
							false,        
							ra->type);    
					add(a);
				}
			}
		}
	}
	end(); 
}





gAction *gActionChannel::getSelectedAction() {
	for (int i=0; i<children(); i++) {
		int action_x  = ((gAction*)child(i))->x();
		int action_w  = ((gAction*)child(i))->w();
		if (Fl::event_x() >= action_x && Fl::event_x() <= action_x + action_w)
			return (gAction*)child(i);
	}
	return NULL;
}





void gActionChannel::updateActions() {

	

	gAction *a;
	for (int i=0; i<children(); i++) {

		a = (gAction*)child(i);
		int newX = x() + (a->frame_a / pParent->zoom);

		if (ch->mode == SINGLE_PRESS) {
			int newW = ((a->frame_b - a->frame_a) / pParent->zoom);
			if (newW < gAction::MIN_WIDTH)
				newW = gAction::MIN_WIDTH;
			a->resize(newX, a->y(), newW, a->h());
		}
		else
			a->resize(newX, a->y(), gAction::MIN_WIDTH, a->h());
	}
}





void gActionChannel::draw() {

	

	baseDraw();

	

	fl_color(COLOR_BG_1);
	fl_font(FL_HELVETICA, 12);
	if (active())
		fl_draw("start/stop", x()+4, y(), w(), h(), (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_CENTER));  
	else
		fl_draw("start/stop (disabled)", x()+4, y(), w(), h(), (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_CENTER));  

	draw_children();
}





int gActionChannel::handle(int e) {

	int ret = Fl_Group::handle(e);

	

	if (!active())
		return 1;

	switch (e) {

		case FL_DRAG: {
			if (selected != NULL) {   

				

				if (selected->onLeftEdge || selected->onRightEdge) {

					

					if (selected->onRightEdge) {

						int aw = Fl::event_x()-selected->x();
						int ah = selected->h();

						if (Fl::event_x() < selected->x()+gAction::MIN_WIDTH)
							aw = gAction::MIN_WIDTH;
						else
						if (Fl::event_x() > pParent->coverX)
							aw = pParent->coverX-selected->x();

						selected->size(aw, ah);
					}
					else {

						int ax = Fl::event_x();
						int ay = selected->y();
						int aw = selected->x()-Fl::event_x()+selected->w();
						int ah = selected->h();

						if (Fl::event_x() < x()) {
							ax = x();
							aw = selected->w()+selected->x()-x();
						}
						else
						if (Fl::event_x() > selected->x()+selected->w()-gAction::MIN_WIDTH) {
							ax = selected->x()+selected->w()-gAction::MIN_WIDTH;
							aw = gAction::MIN_WIDTH;
						}
						selected->resize(ax, ay, aw, ah);
					}
				}

				

				else {
					int real_x = Fl::event_x() - actionPickPoint;
					if (real_x < x())                                  
						selected->position(x(), selected->y());
					else
					if (real_x+selected->w() > pParent->coverX+x())         
						selected->position(pParent->coverX+x()-selected->w(), selected->y());
					else {
						if (pParent->gridTool->isOn()) {
							int snpx = pParent->gridTool->getSnapPoint(real_x-x()) + x() -1;
							selected->position(snpx, selected->y());
						}
						else
							selected->position(real_x, selected->y());
					}
				}
				redraw();
			}
			ret = 1;
			break;
		}

		case FL_PUSH:	{

			if (Fl::event_button1()) {

				

				selected = getSelectedAction();

				if (selected == NULL) {

					

					if (Fl::event_x() >= pParent->coverX) {
						ret = 1;
						break;
					}

					

					int ax = Fl::event_x();
					int fx = (ax - x()) * pParent->zoom;
					if (pParent->gridTool->isOn()) {
						ax = pParent->gridTool->getSnapPoint(ax-x()) + x() -1;
						fx = pParent->gridTool->getSnapFrame(ax-x());

						

						if (actionCollides(fx)) {
							ret = 1;
							break;
						}
					}

					gAction *a = new gAction(
							ax,                                   
							y()+4,                                
							h()-8,                                
							fx,																		
							recorder::frames.size()-1,            
							pParent,                              
							ch,                                   
							true,                                 
							pParent->getActionType());            
					add(a);
					mainWin->keyboard->setChannelWithActions((gSampleChannel*)ch->guiChannel); 
					redraw();
					ret = 1;
				}
				else {
					actionOriginalX = selected->x();
					actionOriginalW = selected->w();
					actionPickPoint = Fl::event_x() - selected->x();
					ret = 1;   
				}
			}
			else
			if (Fl::event_button3()) {
				gAction *a = getSelectedAction();
				if (a != NULL) {
					a->delAction();
					remove(a);
					delete a;
					mainWin->keyboard->setChannelWithActions((gSampleChannel*)pParent->chan->guiChannel);
					redraw();
					ret = 1;
				}
			}
			break;
		}
		case FL_RELEASE: {

			if (selected == NULL) {
				ret = 1;
				break;
			}

			

			bool noChanges = false;
			if (actionOriginalX == selected->x())
				noChanges = true;
			if (ch->mode == SINGLE_PRESS &&
					actionOriginalX+actionOriginalW != selected->x()+selected->w())
				noChanges = false;

			if (noChanges) {
				ret = 1;
				selected = NULL;
				break;
			}

			

			bool overlap = false;
			for (int i=0; i<children() && !overlap; i++) {

				

				if ((gAction*)child(i) == selected)
					continue;

				int action_x  = ((gAction*)child(i))->x();
				int action_w  = ((gAction*)child(i))->w();
				if (ch->mode == SINGLE_PRESS) {

					

					 int start = action_x >= selected->x() ? action_x : selected->x();
					 int end   = action_x+action_w < selected->x()+selected->w() ? action_x+action_w : selected->x()+selected->w();
					 if (start < end) {
						selected->resize(actionOriginalX, selected->y(), actionOriginalW, selected->h());
						redraw();
						overlap = true;   
					}
				}
				else {
					if (selected->x() == action_x) {
						selected->position(actionOriginalX, selected->y());
						redraw();
						overlap = true;   
					}
				}
			}

			

			if (!overlap) {
				if (pParent->gridTool->isOn()) {
					int f = pParent->gridTool->getSnapFrame(selected->absx());
					selected->moveAction(f);
				}
				else
					selected->moveAction();
			}
			selected = NULL;
			ret = 1;
			break;
		}
	}

	return ret;
}





bool gActionChannel::actionCollides(int frame) {

	

	bool collision = false;

	for (int i=0; i<children() && !collision; i++)
		if ( ((gAction*)child(i))->frame_a == frame)
			collision = true;

	if (ch->mode == SINGLE_PRESS) {
		for (int i=0; i<children() && !collision; i++) {
			gAction *c = ((gAction*)child(i));
			if (frame <= c->frame_b && frame >= c->frame_a)
				collision = true;
		}
	}

	return collision;
}







const int gAction::MIN_WIDTH = 8;







gAction::gAction(int X, int Y, int H, int frame_a, unsigned index, gdActionEditor *parent, SampleChannel *ch, bool record, char type)
: Fl_Box     (X, Y, MIN_WIDTH, H),
  selected   (false),
  index      (index),
  parent     (parent),
  ch         (ch),
  type       (type),
  frame_a    (frame_a),
  onRightEdge(false),
  onLeftEdge (false)
{
	

	if (record)
		addAction();

	

	if (ch->mode == SINGLE_PRESS && type == ACTION_KEYPRESS) {
		recorder::action *a2 = NULL;
		recorder::getNextAction(ch->index, ACTION_KEYREL, frame_a, &a2);
		if (a2) {
			frame_b = a2->frame;
			w((frame_b - frame_a)/parent->zoom);
		}
		else
			gLog("[gActionChannel] frame_b not found! [%d:???]\n", frame_a);

	

		if (w() < MIN_WIDTH)
			size(MIN_WIDTH, h());
	}
}





void gAction::draw() {

	int color;
	if (selected)  
		color = COLOR_BD_1;
	else
		color = COLOR_BG_2;

	if (ch->mode == SINGLE_PRESS) {
		fl_rectf(x(), y(), w(), h(), (Fl_Color) color);
	}
	else {
		if (type == ACTION_KILLCHAN)
			fl_rect(x(), y(), MIN_WIDTH, h(), (Fl_Color) color);
		else {
			fl_rectf(x(), y(), MIN_WIDTH, h(), (Fl_Color) color);
			if (type == ACTION_KEYPRESS)
				fl_rectf(x()+3, y()+h()-11, 2, 8, COLOR_BD_0);
			else
			if  (type == ACTION_KEYREL)
				fl_rectf(x()+3, y()+3, 2, 8, COLOR_BD_0);
		}
	}

}





int gAction::handle(int e) {

	

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

			

			if (ch->mode == SINGLE_PRESS) {
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
			}
		}
	}

	return ret;
}





void gAction::addAction() {

	

	if (frame_a % 2 != 0)
		frame_a++;

	

	if (ch->mode == SINGLE_PRESS) {
		recorder::rec(parent->chan->index, ACTION_KEYPRESS, frame_a);
		recorder::rec(parent->chan->index, ACTION_KEYREL, frame_a+4096);
		
	}
	else {
		recorder::rec(parent->chan->index, parent->getActionType(), frame_a);
		
	}

	recorder::sortActions();

	index++; 
}





void gAction::delAction() {

	

	if (ch->mode == SINGLE_PRESS) {
		recorder::deleteAction(parent->chan->index, frame_a, ACTION_KEYPRESS, false);
		recorder::deleteAction(parent->chan->index, frame_b, ACTION_KEYREL, false);
	}
	else
		recorder::deleteAction(parent->chan->index, frame_a, type, false);

	

	fl_cursor(FL_CURSOR_DEFAULT, FL_WHITE, FL_BLACK);
}





void gAction::moveAction(int frame_a) {

	

	delAction();

	if (frame_a != -1)
		this->frame_a = frame_a;
	else
		this->frame_a = xToFrame_a();


	

	if (this->frame_a % 2 != 0)
		this->frame_a++;

	recorder::rec(parent->chan->index, type, this->frame_a);

	if (ch->mode == SINGLE_PRESS) {
		frame_b = xToFrame_b();
		recorder::rec(parent->chan->index, ACTION_KEYREL, frame_b);
	}

	recorder::sortActions();
}





int gAction::absx() {
	return x() - parent->ac->x();
}





int gAction::xToFrame_a() {
	return (absx()) * parent->zoom;
}





int gAction::xToFrame_b() {
	return (absx() + w()) * parent->zoom;
}
