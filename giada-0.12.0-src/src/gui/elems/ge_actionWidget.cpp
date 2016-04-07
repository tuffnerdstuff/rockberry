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
#include "../../core/mixer.h"
#include "../dialogs/gd_actionEditor.h"
#include "ge_actionWidget.h"
#include "ge_mixed.h"


extern Mixer G_Mixer;


gActionWidget::gActionWidget(int x, int y, int w, int h, gdActionEditor *pParent)
	:	Fl_Group(x, y, w, h), pParent(pParent) {}





gActionWidget::~gActionWidget() {}





void gActionWidget::baseDraw(bool clear) {

	

	if (clear)
		fl_rectf(x(), y(), w(), h(), COLOR_BG_MAIN);

	

	fl_color(COLOR_BD_0);
	fl_rect(x(), y(), w(), h());

	

	if (pParent->gridTool->getValue() > 1) {

		fl_color(fl_rgb_color(54, 54, 54));
		fl_line_style(FL_DASH, 0, NULL);

		for (int i=0; i<(int) pParent->gridTool->points.size(); i++) {
			int px = pParent->gridTool->points.at(i)+x()-1;
			fl_line(px, y()+1, px, y()+h()-2);
		}
		fl_line_style(0);
	}

	

	fl_color(COLOR_BD_0);
	for (int i=0; i<(int) pParent->gridTool->beats.size(); i++) {
		int px = pParent->gridTool->beats.at(i)+x()-1;
		fl_line(px, y()+1, px, y()+h()-2);
	}

	fl_color(COLOR_BG_2);
	for (int i=0; i<(int) pParent->gridTool->bars.size(); i++) {
		int px = pParent->gridTool->bars.at(i)+x()-1;
		fl_line(px, y()+1, px, y()+h()-2);
	}

	

	int coverWidth = pParent->totalWidth-pParent->coverX;
	if (coverWidth != 0)
		fl_rectf(pParent->coverX+x(), y()+1, coverWidth, h()-2, COLOR_BG_1);
}
