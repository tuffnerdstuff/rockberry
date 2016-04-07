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




#include "../../core/recorder.h"
#include "../../core/mixer.h"
#include "../../core/channel.h"
#include "../../glue/glue.h"
#include "../../utils/log.h"
#include "../dialogs/gd_actionEditor.h"
#include "../dialogs/gd_mainWindow.h"
#include "ge_keyboard.h"
#include "ge_actionWidget.h"
#include "ge_muteChannel.h"


extern gdMainWindow *mainWin;
extern Mixer         G_Mixer;


gMuteChannel::gMuteChannel(int x, int y, gdActionEditor *pParent)
 : gActionWidget(x, y, 200, 80, pParent), draggedPoint(-1), selectedPoint(-1)
{
	size(pParent->totalWidth, h());
	extractPoints();
}





void gMuteChannel::draw() {

	baseDraw();

	

	fl_color(COLOR_BG_1);
	fl_font(FL_HELVETICA, 12);
	fl_draw("mute", x()+4, y(), w(), h(), (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_CENTER));

	

	fl_color(COLOR_BG_1);
	fl_font(FL_HELVETICA, 9);
	fl_draw("on",  x()+4, y(),        w(), h(), (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_TOP));
	fl_draw("off", x()+4, y()+h()-14, w(), h(), (Fl_Align) (FL_ALIGN_LEFT | FL_ALIGN_TOP));

	

	fl_color(COLOR_BG_2);

	int pxOld = x()+1;
	int pxNew = 0;
	int py    = y()+h()-5;
	int pyDot = py-6;

	for (unsigned i=0; i<points.size(); i++) {

		

		pxNew = points.at(i).x+x();

		

		fl_line(pxOld, py, pxNew, py);
		pxOld = pxNew;

		py = i % 2 == 0 ? y()+4 : y()+h()-5;

		

		fl_line(pxNew, y()+h()-5, pxNew, y()+4);

		if (selectedPoint == (int) i) {
			fl_color(COLOR_BD_1);
			fl_rectf(pxNew-3, pyDot, 7, 7);
			fl_color(COLOR_BG_2);
		}
		else
			fl_rectf(pxNew-3, pyDot, 7, 7);
	}

	

	py = y()+h()-5;
	fl_line(pxNew+3, py, pParent->coverX+x()-1, py);
}





void gMuteChannel::extractPoints() {

	points.clear();

	

	for (unsigned i=0; i<recorder::frames.size(); i++) {
		for (unsigned j=0; j<recorder::global.at(i).size(); j++) {
			if (recorder::global.at(i).at(j)->chan == pParent->chan->index) {
				if (recorder::global.at(i).at(j)->type & (ACTION_MUTEON | ACTION_MUTEOFF)) {
					point p;
					p.frame = recorder::frames.at(i);
					p.type  = recorder::global.at(i).at(j)->type;
					p.x     = p.frame / pParent->zoom;
					points.push_back(p);
					
				}
			}
		}
	}
}





void gMuteChannel::updateActions() {
	for (unsigned i=0; i<points.size(); i++)
		points.at(i).x = points.at(i).frame / pParent->zoom;
}





int gMuteChannel::handle(int e) {

	int ret = 0;
	int mouseX = Fl::event_x()-x();

	switch (e) {

		case FL_ENTER: {
			ret = 1;
			break;
		}

		case FL_MOVE: {
			selectedPoint = getSelectedPoint();
			redraw();
			ret = 1;
			break;
		}

		case FL_LEAVE: {
			draggedPoint  = -1;
			selectedPoint = -1;
			redraw();
			ret = 1;
			break;
		}

		case FL_PUSH: {

			

			if (Fl::event_button1())  {

				if (selectedPoint != -1) {
					draggedPoint   = selectedPoint;
					previousXPoint = points.at(selectedPoint).x;
				}
				else {

					

					if (mouseX > pParent->coverX) {
						ret = 1;
						break;
					}

					

					unsigned nextPoint = points.size();
					for (unsigned i=0; i<points.size(); i++) {
						if (mouseX < points.at(i).x) {
							nextPoint = i;
							break;
						}
					}

					

					int frame_a = mouseX * pParent->zoom;
					int frame_b = frame_a+2048;

					if (pParent->gridTool->isOn()) {
						frame_a = pParent->gridTool->getSnapFrame(mouseX);
						frame_b = pParent->gridTool->getSnapFrame(mouseX + pParent->gridTool->getCellSize());

						

						if (pointCollides(frame_a) || pointCollides(frame_b)) {
							ret = 1;
							break;
						}
					}

					

					if (frame_a % 2 != 0) frame_a++;
					if (frame_b % 2 != 0) frame_b++;

					

					if (frame_b >= G_Mixer.totalFrames) {
						frame_b = G_Mixer.totalFrames;
						frame_a = frame_b-2048;
					}

					if (nextPoint % 2 != 0) {
						recorder::rec(pParent->chan->index, ACTION_MUTEOFF, frame_a);
						recorder::rec(pParent->chan->index, ACTION_MUTEON,  frame_b);
					}
					else {
						recorder::rec(pParent->chan->index, ACTION_MUTEON,  frame_a);
						recorder::rec(pParent->chan->index, ACTION_MUTEOFF, frame_b);
					}
					recorder::sortActions();

					mainWin->keyboard->setChannelWithActions((gSampleChannel*)pParent->chan->guiChannel); 
					extractPoints();
					redraw();
				}
			}
			else {

				

				if (selectedPoint != -1) {

					unsigned a;
					unsigned b;

					if (points.at(selectedPoint).type == ACTION_MUTEOFF) {
						a = selectedPoint-1;
						b = selectedPoint;
					}
					else {
						a = selectedPoint;
						b = selectedPoint+1;
					}

					
					

					recorder::deleteAction(pParent->chan->index, points.at(a).frame,	points.at(a).type, false); 
					recorder::deleteAction(pParent->chan->index,	points.at(b).frame,	points.at(b).type, false); 
					recorder::sortActions();

					mainWin->keyboard->setChannelWithActions((gSampleChannel*)pParent->chan->guiChannel); 
					extractPoints();
					redraw();
				}
			}
			ret = 1;
			break;
		}

		case FL_RELEASE: {

			if (draggedPoint != -1) {

				if (points.at(draggedPoint).x == previousXPoint) {
					
				}
				else {

					int newFrame = points.at(draggedPoint).x * pParent->zoom;

					recorder::deleteAction(
							pParent->chan->index,
							points.at(draggedPoint).frame,
							points.at(draggedPoint).type,
							false);  

					recorder::rec(
							pParent->chan->index,
							points.at(draggedPoint).type,
							newFrame);

					recorder::sortActions();

					points.at(draggedPoint).frame = newFrame;
				}
			}
			draggedPoint  = -1;
			selectedPoint = -1;

			ret = 1;
			break;
		}

		case FL_DRAG: {

			if (draggedPoint != -1) {

				

				int prevPoint;
				int nextPoint;

				if (draggedPoint == 0) {
					prevPoint = 0;
					nextPoint = points.at(draggedPoint+1).x - 1;
					if (pParent->gridTool->isOn())
						nextPoint -= pParent->gridTool->getCellSize();
				}
				else
				if ((unsigned) draggedPoint == points.size()-1) {
					prevPoint = points.at(draggedPoint-1).x + 1;
					nextPoint = pParent->coverX-x();
					if (pParent->gridTool->isOn())
						prevPoint += pParent->gridTool->getCellSize();
				}
				else {
					prevPoint = points.at(draggedPoint-1).x + 1;
					nextPoint = points.at(draggedPoint+1).x - 1;
					if (pParent->gridTool->isOn()) {
						prevPoint += pParent->gridTool->getCellSize();
						nextPoint -= pParent->gridTool->getCellSize();
					}
				}

				if (mouseX <= prevPoint)
					points.at(draggedPoint).x = prevPoint;
				else
				if (mouseX >= nextPoint)
					points.at(draggedPoint).x = nextPoint;
				else
				if (pParent->gridTool->isOn())
					points.at(draggedPoint).x = pParent->gridTool->getSnapPoint(mouseX)-1;
				else
					points.at(draggedPoint).x = mouseX;

				redraw();
			}
			ret = 1;
			break;
		}
	}


	return ret;
}





bool gMuteChannel::pointCollides(int frame) {
	for (unsigned i=0; i<points.size(); i++)
		if (frame == points.at(i).frame)
			return true;
	return false;
}





int gMuteChannel::getSelectedPoint() {

	

	for (unsigned i=0; i<points.size(); i++) {
		if (Fl::event_x() >= points.at(i).x+x()-3 &&
				Fl::event_x() <= points.at(i).x+x()+3)
		return i;
	}
	return -1;
}
