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




#include <limits.h>
#include "../../core/const.h"
#include "../../utils/utils.h"
#include "../../utils/log.h"
#include "ge_browser.h"


gBrowser::gBrowser(int x, int y, int w, int h, const char *L)
 : Fl_Hold_Browser(x, y, w, h, L)
{
	box(G_BOX);
	textsize(GUI_FONT_SIZE_BASE);
	textcolor(COLOR_TEXT_0);
	selection_color(COLOR_BG_1);
	color(COLOR_BG_0);

	this->scrollbar.color(COLOR_BG_0);
	this->scrollbar.selection_color(COLOR_BG_1);
	this->scrollbar.labelcolor(COLOR_BD_1);
	this->scrollbar.slider(G_BOX);

	this->hscrollbar.color(COLOR_BG_0);
	this->hscrollbar.selection_color(COLOR_BG_1);
	this->hscrollbar.labelcolor(COLOR_BD_1);
	this->hscrollbar.slider(G_BOX);
}





gBrowser::~gBrowser() {}





void gBrowser::init(const char *init_path) {

	gLog("[gBrowser] init path = '%s'\n", init_path);

	if (init_path == NULL || !gIsDir(init_path)) {
#if defined(__linux__) || defined(__APPLE__)
		path_obj->value("/home");
#elif defined(_WIN32)

		

		char winRoot[1024];
		SHGetFolderPath(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, winRoot); 
		path_obj->value(winRoot);
#endif
		gLog("[gBrowser] init_path null or invalid, using default\n");
	}
	else
		path_obj->value(init_path);

	refresh();
	sort();
}





void gBrowser::refresh() {
  DIR *dp;
  struct dirent *ep;
  dp = opendir(path_obj->value());
  if (dp != NULL) {
		while ((ep = readdir(dp))) {

			

			if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {
				if (ep->d_name[0] != '.') {

					

					std::string file = path_obj->value();
					file.insert(file.size(), gGetSlash());
					file += ep->d_name;

					if (gIsDir(file.c_str())) {
						char name[PATH_MAX];
						sprintf(name, "@b[%s]", ep->d_name);
						add(name);
					}
					else
					if (gIsProject(file.c_str())) {
						char name[PATH_MAX];
						sprintf(name, "@i@b%s", ep->d_name);
						add(name);
					}
					else
						add(ep->d_name);
				}
			}
		}
		closedir(dp);
  }
  else
    gLog("[gBrowser] Couldn't open the directory '%s'\n", path_obj->value());
}





void gBrowser::sort() {
	for (int t=1; t<=size(); t++)
		for (int r=t+1; r<=size(); r++)
			if (strcmp(text(t), text(r)) > 0)
				swap(t,r);
}





void gBrowser::up_dir() {

	

	int i = strlen(path_obj->value())-1;

	

#if defined(_WIN32)
	if (i <= 3 || !strcmp(path_obj->value(), "All drives")) {
		path_obj->value("All drives");
		showDrives();
		return;
	}
	else {
		while (i >= 0) {
			if (path_obj->value()[i] == '\\')
				break;
			i--;
		}

		

		std::string tmp = path_obj->value();
		tmp.erase(i, tmp.size()-i);

		

		if (tmp.size() == 2)
			tmp += "\\";

		path_obj->value(tmp.c_str());
		refresh();
	}
#elif defined(__linux__) || defined (__APPLE__)
	while (i >= 0) {
		if (path_obj->value()[i] == '/')
			break;
		i--;
	}

	

	if (i==0)
		path_obj->value("/");
	else {

		

		std::string tmp = path_obj->value();
		tmp.erase(i, tmp.size()-i);
		path_obj->value(tmp.c_str());
	}
	refresh();
#endif
}





void gBrowser::down_dir(const char *path) {
	path_obj->value(path);
	refresh();
}





const char *gBrowser::get_selected_item() {

	

	if (text(value()) == NULL)
		return NULL;

	selected_item = text(value());

	

	if (selected_item[0] == '@') {
		if (selected_item[1] == 'b') {
			selected_item.erase(0, 3);
			selected_item.erase(selected_item.size()-1, 1);
		}
		else
		if (selected_item[1] == 'i')
			selected_item.erase(0, 4);
	}

#if defined(__linux__) || defined(__APPLE__)

	

	if (strcmp("/", path_obj->value()))
		selected_item.insert(0, "/");

	selected_item.insert(0, path_obj->value());
	return selected_item.c_str();
#elif defined(_WIN32)

	

	if (strcmp(path_obj->value(), "All drives") == 0)
			return selected_item.c_str();
	else {

		

		if (strlen(path_obj->value()) > 3) 
			selected_item.insert(0, "\\");

		selected_item.insert(0, path_obj->value());
		return selected_item.c_str();
	}
#endif
}





#ifdef _WIN32
void gBrowser::showDrives() {

	

	char drives[64];
	char *i = drives;		
	GetLogicalDriveStrings(64, drives);

	

	while (*i) {
		add(i);
		i = &i[strlen(i) + 1];
	}
}

#endif
