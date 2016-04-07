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




#ifndef MIXERHANDLER_H
#define MIXERHANDLER_H


#include <vector>
#include "recorder.h"
#include "patch.h"


using std::vector;




void mh_stopSequencer();

void mh_rewindSequencer();



bool mh_uniqueSolo(class Channel *ch);



void mh_loadPatch_DEPR_(bool isProject, const char *projPath=0);
void mh_readPatch();



SampleChannel *mh_startInputRec();

SampleChannel *mh_stopInputRec();



bool mh_uniqueSamplename(class SampleChannel *ch, const char *name);

#endif
