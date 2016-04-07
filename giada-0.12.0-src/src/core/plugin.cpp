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




#ifdef WITH_VST


#include "../utils/log.h"
#include "plugin.h"


using std::string;


int Plugin::idGenerator = 1;





void Plugin::initEditor()
{
  if (getActiveEditor() != NULL) {
    gLog("[Plugin::initEditor] plugin has an already active editor!\n");
    return;
  }
  ui = createEditor();
  if (ui == NULL) {
    gLog("[Plugin::initEditor] unable to create editor!\n");
    return;
  }
}





void Plugin::showEditor(void *parent)
{
  if (ui == NULL) {
    gLog("[Plugin::showEditor] can't show editor!\n");
    return;
  }
  ui->setOpaque(true);
  ui->setVisible(true);
  ui->addToDesktop(0, parent);
}





bool Plugin::isEditorOpen()
{
  return ui->isVisible();
}





string Plugin::getUniqueId()
{
  return getPluginDescription().fileOrIdentifier.toStdString();
}





void Plugin::closeEditor()
{
  if (ui == NULL)
    return;
  ui->setVisible(false);
  if (ui->isOnDesktop())
  	ui->removeFromDesktop();
}

#endif
