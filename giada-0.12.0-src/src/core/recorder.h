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



#ifndef RECORDER_H
#define RECORDER_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../utils/utils.h"
#include "const.h"
#include "mixer.h"

#ifdef WITH_VST



	#ifdef __GNUC__
		#ifndef _WIN32
			#define __cdecl
		#endif
	#endif
	#include "../deps/vst/aeffectx.h"
#endif


using std::vector;




namespace recorder {



struct action {
	int      chan;    
 	int      type;
	int      frame;   
	float    fValue;  
	uint32_t iValue;  
};



struct composite {
	action a1;
	action a2;
};

extern vector<int>  frames;					      
extern vector< vector<action*> > global;	
extern vector<action*>  actions;				    

extern bool active;
extern bool sortedActions;                  



void init();



void chanHasActions(int chan);



bool canRec(Channel *ch);



void rec(int chan, int action, int frame, uint32_t iValue=0, float fValue=0.0f);



void clearChan(int chan);



void clearAction(int chan, char action);



void deleteAction(int chan, int frame, char type, bool checkValues, uint32_t iValue=0, float fValue=0.0);



void deleteActions(int chan, int frame_a, int frame_b, char type);



void clearAll();



void optimize();



void sortActions();



void updateBpm(float oldval, float newval, int oldquanto);



void updateSamplerate(int systemRate, int patchRate);

void expand(int old_fpb, int new_fpb);
void shrink(int new_fpb);



int getNextAction(int chan, char action, int frame, struct action **out, uint32_t iValue=0);



int getAction(int chan, char action, int frame, struct action **out);



void startOverdub(int chan, char action, int frame);
void stopOverdub(int frame);



void print();

}  

#endif
