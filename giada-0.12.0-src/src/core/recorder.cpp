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




#include <math.h>
#include "recorder.h"
#include "const.h"
#include "mixer.h"
#include "mixerHandler.h"
#include "kernelAudio.h"
#include "kernelMidi.h"
#include "patch_DEPR_.h"
#include "conf.h"
#include "channel.h"
#include "sampleChannel.h"
#include "../utils/log.h"
#include "../utils/utils.h"




extern Mixer       G_Mixer;
extern Patch_DEPR_ f_patch;
extern Conf	       G_Conf;


namespace recorder
{
vector<int> frames;
vector< vector<action*> > global;
vector<action*>  actions;

bool active = false;
bool sortedActions = false;

composite cmp;





void init()
{
	sortedActions = false;
	active = false;
	clearAll();
}





bool canRec(Channel *ch)
{
	

	if (!active || !G_Mixer.running || G_Mixer.chanInput == ch || (ch->type == CHANNEL_SAMPLE && ((SampleChannel*)ch)->wave == NULL))
		return 0;
	return 1;
}





void rec(int index, int type, int frame, uint32_t iValue, float fValue)
{
	

	if (frame % 2 != 0)
		frame++;

	

	action *a = (action*) malloc(sizeof(action));
	a->chan   = index;
	a->type   = type;
	a->frame  = frame;
	a->iValue = iValue;
	a->fValue = fValue;

	

	int frameToExpand = frames.size();
	for (int i=0; i<frameToExpand; i++)
		if (frames.at(i) == frame) {
			frameToExpand = i;
			break;
		}

	

	if (frameToExpand == (int) frames.size()) {
		frames.push_back(frame);
		global.push_back(actions);               
		global.at(global.size()-1).push_back(a); 
	}
	else {

		

		for (unsigned t=0; t<global.at(frameToExpand).size(); t++) {
			action *ac = global.at(frameToExpand).at(t);
			if (ac->chan   == index  &&
			    ac->type   == type   &&
			    ac->frame  == frame  &&
			    ac->iValue == iValue &&
			    ac->fValue == fValue)
				return;
		}

		global.at(frameToExpand).push_back(a);		
	}

	

	Channel *ch = G_Mixer.getChannelByIndex(index);
	ch->hasActions = true;

	sortedActions = false;

	gLog("[REC] action recorded, type=%d frame=%d chan=%d iValue=%d (%X) fValue=%f\n",
		a->type, a->frame, a->chan, a->iValue, a->iValue, a->fValue);
	
}





void clearChan(int index)
{
	gLog("[REC] clearing chan %d...\n", index);

	for (unsigned i=0; i<global.size(); i++) {	
		unsigned j=0;
		while (true) {
			if (j == global.at(i).size()) break; 	  
			action *a = global.at(i).at(j);
			if (a->chan == index)	{
#ifdef WITH_VST
#if 0
				if (a->type == ACTION_MIDI)
					free(a->event);
#endif
#endif
				free(a);
				global.at(i).erase(global.at(i).begin() + j);
			}
			else
				j++;
		}
	}

	Channel *ch = G_Mixer.getChannelByIndex(index);
	ch->hasActions = false;
	optimize();
	
}





void clearAction(int index, char act)
{
	gLog("[REC] clearing action %d from chan %d...\n", act, index);
	for (unsigned i=0; i<global.size(); i++) {						
		unsigned j=0;
		while (true) {                                   
			if (j == global.at(i).size())
				break;
			action *a = global.at(i).at(j);
			if (a->chan == index && (act & a->type) == a->type)	{ 
				free(a);
				global.at(i).erase(global.at(i).begin() + j);
			}
			else
				j++;
		}
	}
	Channel *ch = G_Mixer.getChannelByIndex(index);
	ch->hasActions = false;   
	optimize();
	chanHasActions(index);    
	
}





void deleteAction(int chan, int frame, char type, bool checkValues, uint32_t iValue, float fValue)
{
	

	if (frame % 2 != 0)
		frame++;

	

	bool found = false;
	for (unsigned i=0; i<frames.size() && !found; i++) {
		if (frames.at(i) == frame) {

			

			for (unsigned j=0; j<global.at(i).size(); j++) {
				action *a = global.at(i).at(j);

				

				bool doit = (a->chan == chan && a->type == (type & a->type));
				if (checkValues)
					doit &= (a->iValue == iValue && a->fValue == fValue);

				if (doit) {
					int lockStatus = 0;
					while (lockStatus == 0) {
						lockStatus = pthread_mutex_trylock(&G_Mixer.mutex_recs);
						if (lockStatus == 0) {
#ifdef WITH_VST
#if 0
							if (type == ACTION_MIDI)
								free(a->event);
#endif
#endif
							free(a);
							global.at(i).erase(global.at(i).begin() + j);
							pthread_mutex_unlock(&G_Mixer.mutex_recs);
							found = true;
							break;
						}
						else
							gLog("[REC] delete action: waiting for mutex...\n");
					}
				}
			}
		}
	}
	if (found) {
		optimize();
		chanHasActions(chan);
		gLog("[REC] action deleted, type=%d frame=%d chan=%d iValue=%d (%X) fValue=%f\n",
			type, frame, chan, iValue, iValue, fValue);
	}
	else
		gLog("[REC] unable to delete action, not found! type=%d frame=%d chan=%d iValue=%d (%X) fValue=%f\n",
			type, frame, chan, iValue, iValue, fValue);
}





void deleteActions(int chan, int frame_a, int frame_b, char type)
{
	sortActions();
	vector<int> dels;

	for (unsigned i=0; i<frames.size(); i++)
		if (frames.at(i) > frame_a && frames.at(i) < frame_b)
			dels.push_back(frames.at(i));

	for (unsigned i=0; i<dels.size(); i++)
		deleteAction(chan, dels.at(i), type, false); 
}





void clearAll()
{
	while (global.size() > 0) {
		for (unsigned i=0; i<global.size(); i++) {
			for (unsigned k=0; k<global.at(i).size(); k++) {
#ifdef WITH_VST
#if 0
				if (global.at(i).at(k)->type == ACTION_MIDI)
					free(global.at(i).at(k)->event);
#endif
#endif
				free(global.at(i).at(k));									
			}
			global.at(i).clear();												
			global.erase(global.begin() + i);
		}
	}

	for (unsigned i=0; i<G_Mixer.channels.size(); i++) {
		G_Mixer.channels.at(i)->hasActions  = false;
		if (G_Mixer.channels.at(i)->type == CHANNEL_SAMPLE)
			((SampleChannel*)G_Mixer.channels.at(i))->readActions = false;
	}

	global.clear();
	frames.clear();
}





void optimize()
{
	

	unsigned i = 0;
	while (true) {
		if (i == global.size()) return;
		if (global.at(i).size() == 0) {
			global.erase(global.begin() + i);
			frames.erase(frames.begin() + i);
		}
		else
			i++;
	}

	sortActions();
}





void sortActions()
{
	if (sortedActions)
		return;
	for (unsigned i=0; i<frames.size(); i++)
		for (unsigned j=0; j<frames.size(); j++)
			if (frames.at(j) > frames.at(i)) {
				std::swap(frames.at(j), frames.at(i));
				std::swap(global.at(j), global.at(i));
			}
	sortedActions = true;
	
}





void updateBpm(float oldval, float newval, int oldquanto)
{
	for (unsigned i=0; i<frames.size(); i++) {

		float frame  = ((float) frames.at(i)/newval) * oldval;
		frames.at(i) = (int) frame;

		
		

		if (frames.at(i) != 0) {
			int scarto = oldquanto % frames.at(i);
			if (scarto > 0 && scarto <= 6)
				frames.at(i) = frames.at(i) + scarto;
		}

		

		if (frames.at(i) % 2 != 0)
			frames.at(i)++;
	}

	

	for (unsigned i=0; i<frames.size(); i++) {
		for (unsigned j=0; j<global.at(i).size(); j++) {
			action *a = global.at(i).at(j);
			a->frame = frames.at(i);
		}
	}

	
}





void updateSamplerate(int systemRate, int patchRate)
{
	

	if (systemRate == patchRate)
		return;

	gLog("[REC] systemRate (%d) != patchRate (%d), converting...\n", systemRate, patchRate);

	float ratio = systemRate / (float) patchRate;
	for (unsigned i=0; i<frames.size(); i++) {

		gLog("[REC]    oldFrame = %d", frames.at(i));

		float newFrame = frames.at(i);
		newFrame = floorf(newFrame * ratio);

		frames.at(i) = (int) newFrame;

		if (frames.at(i) % 2 != 0)
			frames.at(i)++;

		gLog(", newFrame = %d\n", frames.at(i));
	}

	

	for (unsigned i=0; i<frames.size(); i++) {
		for (unsigned j=0; j<global.at(i).size(); j++) {
			action *a = global.at(i).at(j);
			a->frame = frames.at(i);
		}
	}
}





void expand(int old_fpb, int new_fpb)
{
	

	unsigned pass = (int) (new_fpb / old_fpb) - 1;
	if (pass == 0) pass = 1;

	unsigned init_fs = frames.size();

	for (unsigned z=1; z<=pass; z++) {
		for (unsigned i=0; i<init_fs; i++) {
			unsigned newframe = frames.at(i) + (old_fpb*z);
			frames.push_back(newframe);
			global.push_back(actions);
			for (unsigned k=0; k<global.at(i).size(); k++) {
				action *a = global.at(i).at(k);
				rec(a->chan, a->type, newframe, a->iValue, a->fValue);
			}
		}
	}
	gLog("[REC] expanded recs\n");
	
}





void shrink(int new_fpb)
{
	

	unsigned i=0;
	while (true) {
		if (i == frames.size()) break;

		if (frames.at(i) >= new_fpb) {
			for (unsigned k=0; k<global.at(i).size(); k++)
				free(global.at(i).at(k));		      
			global.at(i).clear();								
			global.erase(global.begin() + i);   
			frames.erase(frames.begin() + i);   
		}
		else
			i++;
	}
	optimize();
	gLog("[REC] shrinked recs\n");
	
}





void chanHasActions(int index)
{
	Channel *ch = G_Mixer.getChannelByIndex(index);
	if (global.size() == 0) {
		ch->hasActions = false;
		return;
	}
	for (unsigned i=0; i<global.size() && !ch->hasActions; i++) {
		for (unsigned j=0; j<global.at(i).size() && !ch->hasActions; j++) {
			if (global.at(i).at(j)->chan == index)
				ch->hasActions = true;
		}
	}
}





int getNextAction(int chan, char type, int frame, action **out, uint32_t iValue)
{
	sortActions();  

	unsigned i=0;
	while (i < frames.size() && frames.at(i) <= frame) i++;

	if (i == frames.size())   
		return -1;

	for (; i<global.size(); i++)
		for (unsigned j=0; j<global.at(i).size(); j++) {
			action *a = global.at(i).at(j);
			if (a->chan == chan && (type & a->type) == a->type) {
				if (iValue == 0 || (iValue != 0 && a->iValue == iValue)) {
					*out = global.at(i).at(j);
					return 1;
				}
			}
		}

	return -2;   
}





int getAction(int chan, char action, int frame, struct action **out)
{
	for (unsigned i=0; i<global.size(); i++)
		for (unsigned j=0; j<global.at(i).size(); j++)
			if (frame  == global.at(i).at(j)->frame &&
					action == global.at(i).at(j)->type &&
					chan   == global.at(i).at(j)->chan)
			{
				*out = global.at(i).at(j);
				return 1;
			}
	return 0;
}





void startOverdub(int index, char actionMask, int frame)
{
	

	if (actionMask == ACTION_KEYS) {
		cmp.a1.type = ACTION_KEYPRESS;
		cmp.a2.type = ACTION_KEYREL;
	}
	else {
		cmp.a1.type = ACTION_MUTEON;
		cmp.a2.type = ACTION_MUTEOFF;
	}
	cmp.a1.chan  = index;
	cmp.a2.chan  = index;
	cmp.a1.frame = frame;
	

	

	rec(index, cmp.a1.type, frame);

	action *act = NULL;
	int res = getNextAction(index, cmp.a1.type | cmp.a2.type, cmp.a1.frame, &act);
	if (res == 1) {
		if (act->type == cmp.a2.type) {
			int truncFrame = cmp.a1.frame-kernelAudio::realBufsize;
			if (truncFrame < 0)
				truncFrame = 0;
			gLog("[REC] add truncation at frame %d, type=%d\n", truncFrame, cmp.a2.type);
			rec(index, cmp.a2.type, truncFrame);
		}
	}

	SampleChannel *ch = (SampleChannel*) G_Mixer.getChannelByIndex(index);
	ch->readActions = false;   
}





void stopOverdub(int frame)
{
	cmp.a2.frame  = frame;
	bool ringLoop = false;
	bool nullLoop = false;

	

	if (cmp.a2.frame < cmp.a1.frame) {
		ringLoop = true;
		gLog("[REC] ring loop! frame1=%d < frame2=%d\n", cmp.a1.frame, cmp.a2.frame);
		rec(cmp.a2.chan, cmp.a2.type, G_Mixer.totalFrames); 	
	}
	else
	if (cmp.a2.frame == cmp.a1.frame) {
		nullLoop = true;
		gLog("[REC]  null loop! frame1=%d == frame2=%d\n", cmp.a1.frame, cmp.a2.frame);
		deleteAction(cmp.a1.chan, cmp.a1.frame, cmp.a1.type, false); 
	}

	SampleChannel *ch = (SampleChannel*) G_Mixer.getChannelByIndex(cmp.a2.chan);
	ch->readActions = false;      

	

	if (!nullLoop)
		deleteActions(cmp.a2.chan, cmp.a1.frame, cmp.a2.frame, cmp.a1.type);
		deleteActions(cmp.a2.chan, cmp.a1.frame, cmp.a2.frame, cmp.a2.type);

	if (!ringLoop && !nullLoop) {
		rec(cmp.a2.chan, cmp.a2.type, cmp.a2.frame);

		

		action *act = NULL;
		int res = getNextAction(cmp.a2.chan, cmp.a1.type | cmp.a2.type, cmp.a2.frame, &act);
		if (res == 1) {
			if (act->type == cmp.a2.type) {
				gLog("[REC] add truncation at frame %d, type=%d\n", act->frame, act->type);
				deleteAction(act->chan, act->frame, act->type, false); 
			}
		}
	}
}





void print()
{
	gLog("[REC] ** print debug **\n");
	for (unsigned i=0; i<global.size(); i++) {
		gLog("  frame %d\n", frames.at(i));
		for (unsigned j=0; j<global.at(i).size(); j++) {
			gLog("    action %d | chan %d | frame %d\n", global.at(i).at(j)->type, global.at(i).at(j)->chan, global.at(i).at(j)->frame);
		}
	}
}

} 
