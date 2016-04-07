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




#ifndef GLUE_H
#define GLUE_H




void glue_keyPress  (class Channel       *ch, bool ctrl=0, bool shift=0);
void glue_keyPress  (class SampleChannel *ch, bool ctrl=0, bool shift=0);
void glue_keyPress  (class MidiChannel   *ch, bool ctrl=0, bool shift=0);
void glue_keyRelease(class Channel       *ch, bool ctrl=0, bool shift=0);
void glue_keyRelease(class SampleChannel *ch, bool ctrl=0, bool shift=0);

void glue_setBpm(const char *v1, const char *v2);
void glue_setBeats(int beats, int bars, bool expand);



void glue_startStopSeq(bool gui=true);
void glue_startSeq    (bool gui=true);
void glue_stopSeq     (bool gui=true);
void glue_rewindSeq   ();



void glue_startStopActionRec();
void glue_startActionRec();
void glue_stopActionRec();



void glue_startStopInputRec(bool gui=true, bool alert=true);
int  glue_startInputRec    (bool gui=true);
int  glue_stopInputRec     (bool gui=true);



void glue_startStopReadingRecs(class SampleChannel *ch, bool gui=true);
void glue_startReadingRecs    (class SampleChannel *ch, bool gui=true);
void glue_stopReadingRecs     (class SampleChannel *ch, bool gui=true);

void glue_quantize(int val);

void glue_setChanVol(class Channel *ch, float v, bool gui=true);
void glue_setOutVol (float v, bool gui=true);
void glue_setInVol  (float v, bool gui=true);

void glue_setPanning(class gdEditor *win, class SampleChannel *ch, float val);

void glue_clearAllSamples();
void glue_clearAllRecs();



void glue_resetToInitState(bool resetGui=true, bool createColumns=true);

void glue_startStopMetronome(bool gui=true);




void glue_setBeginEndChannel(class gdEditor *win, class SampleChannel *ch, int b, int e,
														 bool recalc=false, bool check=true);


void glue_setBoost(class gdEditor *win, class SampleChannel *ch, float val, bool numeric);

void glue_setPitch(class gdEditor *win, class SampleChannel *ch, float val, bool numeric);



 
void glue_setVolEditor(class gdEditor *win, class SampleChannel *ch, float val, bool numeric);



void glue_setMute(class Channel *ch, bool gui=true);



void glue_setSoloOn (class Channel *ch, bool gui=true);
void glue_setSoloOff(class Channel *ch, bool gui=true);



void glue_beatsMultiply();
void glue_beatsDivide();

#endif
