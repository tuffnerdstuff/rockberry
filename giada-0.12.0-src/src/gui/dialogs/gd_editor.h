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



#ifndef GD_EDITOR_H
#define GD_EDITOR_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <math.h>
#include "../elems/ge_window.h"


class gdEditor : public gWindow {

private:

	static void cb_setChanPos    (Fl_Widget *w, void *p);
	static void cb_resetStartEnd (Fl_Widget *w, void *p);
	static void cb_setVolume     (Fl_Widget *w, void *p);
	static void cb_setVolumeNum  (Fl_Widget *w, void *p);
	static void cb_setBoost      (Fl_Widget *w, void *p);
	static void cb_setBoostNum   (Fl_Widget *w, void *p);
	static void cb_normalize     (Fl_Widget *w, void *p);
	static void cb_panning       (Fl_Widget *w, void *p);
	static void cb_reload        (Fl_Widget *w, void *p);
	static void cb_setPitch      (Fl_Widget *w, void *p);
	static void cb_setPitchToBar (Fl_Widget *w, void *p);
	static void cb_setPitchToSong(Fl_Widget *w, void *p);
	static void cb_setPitchHalf  (Fl_Widget *w, void *p);
	static void cb_setPitchDouble(Fl_Widget *w, void *p);
	static void cb_resetPitch    (Fl_Widget *w, void *p);
	static void cb_setPitchNum   (Fl_Widget *w, void *p);
	static void cb_zoomIn        (Fl_Widget *w, void *p);
	static void cb_zoomOut       (Fl_Widget *w, void *p);
	static void cb_changeGrid    (Fl_Widget *w, void *p);
	static void cb_enableSnap    (Fl_Widget *w, void *p);
	inline void __cb_setChanPos();
	inline void __cb_resetStartEnd();
	inline void __cb_setVolume();
	inline void __cb_setVolumeNum();
	inline void __cb_setBoost();
	inline void __cb_setBoostNum();
	inline void __cb_normalize();
	inline void __cb_panning();
	inline void __cb_reload();
	inline void __cb_setPitch();
	inline void __cb_setPitchToBar();
	inline void __cb_setPitchToSong();
	inline void __cb_setPitchHalf();
	inline void __cb_setPitchDouble();
	inline void __cb_resetPitch();
	inline void __cb_setPitchNum();
	inline void __cb_zoomIn();
	inline void __cb_zoomOut();
	inline void __cb_changeGrid();
	inline void __cb_enableSnap();

public:

	gdEditor(class SampleChannel *ch);
	~gdEditor();

	class gClick     *zoomIn;
	class gClick     *zoomOut;
	class gWaveTools *waveTools;
	class gInput     *chanStart;
	class gInput     *chanEnd;
	class gClick		 *resetStartEnd;
	class gDial      *volume;
	class gInput     *volumeNum;
	class gDial      *boost;
	class gInput     *boostNum;
	class gClick     *normalize;
	class gDial      *pan;
	class gInput     *panNum;
	class gClick		 *reload;
	class gDial  		 *pitch;
	class gInput  	 *pitchNum;
	class gClick  	 *pitchToBar;
	class gClick  	 *pitchToSong;
	class gClick  	 *pitchHalf;
	class gClick  	 *pitchDouble;
	class gClick  	 *pitchReset;
	class gClick     *close;
	class gChoice    *grid;
	class gCheck     *snap;

	class SampleChannel *ch;
};

#endif
