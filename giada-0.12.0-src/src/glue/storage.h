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




#ifndef GLUE_STORAGE_H
#define GLUE_STORAGE_H


#include <vector>
#include "../core/patch.h"


using std::string;
using std::vector;


int glue_loadPatch  (const string &fullPath, class gProgress *status, bool isProject);
int glue_loadPatch__DEPR__(const char *fname, const char *fpath, class gProgress *status, bool isProject);
int glue_savePatch  (const string &fullPath, const string &name, bool isProject);
int glue_saveProject(const string &folderPath, const string &projName);

#endif
