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




#ifndef WAVE_H
#define WAVE_H


#include <samplerate.h>
#include <sndfile.h>
#include <string>


class Wave
{
private:

	SNDFILE   *fileIn;
	SNDFILE   *fileOut;
	SF_INFO    inHeader;
	SF_INFO    outHeader;


public:

	Wave();
	~Wave();
	Wave(const Wave &other);

	std::string pathfile; 
	std::string name;			

	float     *data;
	int        size;			  
	bool       isLogical;   
	bool       isEdited;    

	inline int  rate    () { return inHeader.samplerate; }
	inline int  channels() { return inHeader.channels; }
	inline int  frames  () { return inHeader.frames; }
	inline void rate    (int v) { inHeader.samplerate = v; }
	inline void channels(int v) { inHeader.channels = v; }
	inline void frames  (int v) { inHeader.frames = v; }

	std::string basename () const;
	std::string extension() const;

	void updateName(const char *n);
	int  open      (const char *f);
	int  readData  ();
	int	 writeData (const char *f);
	void clear     ();

	

	int allocEmpty(unsigned size, unsigned samplerate);

	

	int resample(int quality, int newRate);
};

#endif
