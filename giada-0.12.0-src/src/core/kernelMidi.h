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




#ifndef KERNELMIDI_H
#define KERNELMIDI_H


#include <stdint.h>
#include <RtMidi.h>
#include "channel.h"


using std::string;


namespace kernelMidi {

	extern int      api;      
	extern unsigned numOutPorts;
	extern unsigned numInPorts;

	typedef void (cb_midiLearn) (uint32_t, void *);

	

	extern cb_midiLearn *cb_learn;
	extern void         *cb_data;

	void startMidiLearn(cb_midiLearn *cb, void *data);
	void stopMidiLearn();

	inline int getB1(uint32_t iValue) { return (iValue >> 24) & 0xFF; }
	inline int getB2(uint32_t iValue) { return (iValue >> 16) & 0xFF; }
	inline int getB3(uint32_t iValue) { return (iValue >> 8)  & 0xFF; }

	inline uint32_t getIValue(int b1, int b2, int b3) {
		return (b1 << 24) | (b2 << 16) | (b3 << 8) | (0x00);
	}

	

	void send(uint32_t s);

	

	void send(int b1, int b2=-1, int b3=-1);

	

	void setApi(int api);

	

	int openOutDevice(int port);
	int openInDevice(int port);
	int closeInDevice();
	int closeOutDevice();

	

	string getInPortName(unsigned p);
	string getOutPortName(unsigned p);

	bool hasAPI(int API);

	

	void callback(double t, std::vector<unsigned char> *msg, void *data);

	string getRtMidiVersion();
}

#endif
