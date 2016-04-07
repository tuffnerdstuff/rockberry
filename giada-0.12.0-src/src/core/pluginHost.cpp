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
#include "../utils/utils.h"
#include "mixer.h"
#include "channel.h"
#include "midiChannel.h"
#include "plugin.h"
#include "pluginHost.h"


using std::string;


void PluginHost::init(int _buffersize, int _samplerate)
{
  gLog("[PluginHost::init] initialize with buffersize=%d, samplerate=%d\n",
    _buffersize, _samplerate);

  messageManager = juce::MessageManager::getInstance();
  audioBuffer.setSize(2, _buffersize);
  samplerate = _samplerate;
  buffersize = _buffersize;
  loadList(gGetHomePath() + gGetSlash() + "plugins.xml");
}





int PluginHost::scanDir(const string &dirpath, void (*callback)())
{
  gLog("[PluginHost::scanDir] requested directory: '%s'\n", dirpath.c_str());
  gLog("[PluginHost::scanDir] current plugins: %d\n", knownPluginList.getNumTypes());

  knownPluginList.clear();   

  juce::VSTPluginFormat format;
  juce::FileSearchPath path(dirpath);
  juce::PluginDirectoryScanner scanner(knownPluginList, format, path,
      true, juce::File::nonexistent); 

  bool cont = true;
  juce::String name;
  while (cont) {
    gLog("[PluginHost::scanDir]   scanning '%s'\n", name.toRawUTF8());
    cont = scanner.scanNextFile(false, name);
    if (callback)
      callback();
  }

  gLog("[PluginHost::scanDir] %d plugin(s) found\n", knownPluginList.getNumTypes());
  return knownPluginList.getNumTypes();
}





int PluginHost::saveList(const string &filepath)
{
  int out = knownPluginList.createXml()->writeToFile(juce::File(filepath), "");
  if (!out)
    gLog("[PluginHost::saveList] unable to save plugin list to %s\n", filepath.c_str());
  return out;
}





int PluginHost::loadList(const string &filepath)
{
  juce::XmlElement *elem = juce::XmlDocument::parse(juce::File(filepath));
  if (elem) {
    knownPluginList.recreateFromXml(*elem);
    delete elem;
    return 1;
  }
  return 0;
}





Plugin *PluginHost::addPlugin(const string &fid, int stackType,
  pthread_mutex_t *mutex, class Channel *ch)
{
  

  vector <Plugin *> *pStack;
	pStack = getStack(stackType, ch);

  

  juce::PluginDescription *pd = knownPluginList.getTypeForFile(fid);
  if (!pd) {
    gLog("[PluginHost::addPlugin] no plugin found with fid=%s!\n", fid.c_str());
    return NULL;
  }

  Plugin *p = (Plugin *) pluginFormat.createInstanceFromDescription(*pd, samplerate, buffersize);
  if (!p) {
    gLog("[PluginHost::addPlugin] unable to create instance with fid=%s!\n", fid.c_str());
    return NULL;
  }

  p->setStatus(1);
  p->setId();
  p->prepareToPlay(samplerate, buffersize);

  gLog("[PluginHost::addPlugin] plugin instance with fid=%s created\n", fid.c_str());

  

  int lockStatus;
  while (true) {
    lockStatus = pthread_mutex_trylock(mutex);
    if (lockStatus == 0) {
      pStack->push_back(p);
      pthread_mutex_unlock(mutex);
      break;
    }
  }

  gLog("[PluginHost::addPlugin] plugin id=%s loaded (%s), stack type=%d, stack size=%d\n",
    fid.c_str(), p->getName().toStdString().c_str(), stackType, pStack->size());

  return p;
}





Plugin *PluginHost::addPlugin(int index, int stackType, pthread_mutex_t *mutex,
  class Channel *ch)
{
  juce::PluginDescription *pd = knownPluginList.getType(index);
  if (pd) {
    gLog("[PluginHost::addPlugin] plugin found, uid=%s, name=%s...\n",
      pd->fileOrIdentifier.toStdString().c_str(), pd->name.toStdString().c_str());
    return addPlugin(pd->fileOrIdentifier.toStdString(), stackType, mutex, ch);
  }
  else {
    gLog("[PluginHost::addPlugin] no plugins found at index=%d!\n", index);
    return NULL;
  }
}





vector <Plugin *> *PluginHost::getStack(int stackType, Channel *ch)
{
	switch(stackType) {
		case MASTER_OUT:
			return &masterOut;
		case MASTER_IN:
			return &masterIn;
		case CHANNEL:
			return &ch->plugins;
		default:
			return NULL;
	}
}





unsigned PluginHost::countPlugins(int stackType, Channel *ch)
{
	vector <Plugin *> *pStack = getStack(stackType, ch);
	return pStack->size();
}





int PluginHost::countAvailablePlugins()
{
  return knownPluginList.getNumTypes();
}





PluginHost::PluginInfo PluginHost::getAvailablePluginInfo(int i)
{
  juce::PluginDescription *pd = knownPluginList.getType(i);
  PluginInfo pi;
  pi.uid = pd->fileOrIdentifier.toStdString();
  pi.name = pd->name.toStdString();
  pi.category = pd->category.toStdString();
  pi.manufacturerName = pd->manufacturerName.toStdString();
  pi.format = pd->pluginFormatName.toStdString();
  pi.isInstrument = pd->isInstrument;

  return pi;
}





void PluginHost::freeStack(int stackType, pthread_mutex_t *mutex, Channel *ch)
{
	vector <Plugin *> *pStack;
	pStack = getStack(stackType, ch);

	if (pStack->size() == 0)
		return;

	int lockStatus;
	while (true) {
		lockStatus = pthread_mutex_trylock(mutex);
		if (lockStatus == 0) {
			for (unsigned i=0; i<pStack->size(); i++) {
        pStack->at(i)->suspendProcessing(true);
        pStack->at(i)->releaseResources();
				delete pStack->at(i);
			}
			pStack->clear();
			pthread_mutex_unlock(mutex);
			break;
		}
	}
  gLog("[PluginHost::freeStack] stack type=%d freed\n", stackType);
}





void PluginHost::processStack(float *buffer, int stackType, Channel *ch)
{
	vector <Plugin *> *pStack = getStack(stackType, ch);

	

	if (pStack == NULL || pStack->size() == 0)
		return;

	

	for (int i=0; i<buffersize; i++) {
    audioBuffer.setSample(0, i, buffer[i*2]);
    audioBuffer.setSample(1, i, buffer[(i*2)+1]);
	}

	

	for (unsigned i=0; i<pStack->size(); i++) {
    Plugin *plugin = pStack->at(i);
		if (plugin->getStatus() != 1 || plugin->isSuspended() || plugin->isBypassed())
			continue;
    juce::MidiBuffer midiBuffer;
    if (ch && ch->type == CHANNEL_MIDI) { 
      
      midiBuffer = ((MidiChannel*)ch)->getPluginMidiEvents();
    }
		plugin->processBlock(audioBuffer, midiBuffer);
  }

	

	for (int i=0; i<buffersize; i++) {
		buffer[i*2]     = audioBuffer.getSample(0, i);
		buffer[(i*2)+1] = audioBuffer.getSample(1, i);
	}
}





Plugin *PluginHost::getPluginByIndex(int index, int stackType, Channel *ch)
{
	vector <Plugin *> *pStack = getStack(stackType, ch);
	if (pStack->size() == 0)
		return NULL;
	if ((unsigned) index >= pStack->size())
		return NULL;
	return pStack->at(index);
}





int PluginHost::getPluginIndex(int id, int stackType, Channel *ch)
{
	vector <Plugin *> *pStack = getStack(stackType, ch);
	for (unsigned i=0; i<pStack->size(); i++)
		if (pStack->at(i)->getId() == id)
			return i;
	return -1;
}





void PluginHost::swapPlugin(unsigned indexA, unsigned indexB, int stackType,
  pthread_mutex_t *mutex, Channel *ch)
{
	vector <Plugin *> *pStack = getStack(stackType, ch);
	int lockStatus;
	while (true) {
		lockStatus = pthread_mutex_trylock(mutex);
		if (lockStatus == 0) {
			std::swap(pStack->at(indexA), pStack->at(indexB));
			pthread_mutex_unlock(mutex);
			gLog("[pluginHost::swapPlugin] plugin at index %d and %d swapped\n", indexA, indexB);
			return;
		}
		
			
	}
}





void PluginHost::freePlugin(int id, int stackType, pthread_mutex_t *mutex,
  Channel *ch)
{
	vector <Plugin *> *pStack;
	pStack = getStack(stackType, ch);

	

	for (unsigned i=0; i<pStack->size(); i++)
		if (pStack->at(i)->getId() == id) {

			if (pStack->at(i)->getStatus() == 0) { 
				delete pStack->at(i);
				pStack->erase(pStack->begin() + i);
        gLog("[pluginHost::freePlugin] plugin id=%d removed with no frills, since it had status=0\n", id);
				return;
			}
			else {
				int lockStatus;
				while (true) {
					lockStatus = pthread_mutex_trylock(mutex);
					if (lockStatus == 0) {
            pStack->at(i)->suspendProcessing(true);
            pStack->at(i)->releaseResources();
						delete pStack->at(i);
						pStack->erase(pStack->begin() + i);
						pthread_mutex_unlock(mutex);
						gLog("[pluginHost::freePlugin] plugin id=%d removed\n", id);
						return;
					}
					
						
				}
			}
		}
	gLog("[pluginHost::freePlugin] plugin id=%d not found\n", id);
}





void PluginHost::runDispatchLoop()
{
  messageManager->runDispatchLoopUntil(10);
  
}





void PluginHost::freeAllStacks(vector <Channel*> *channels, pthread_mutex_t *mutex)
{
	freeStack(PluginHost::MASTER_OUT, mutex);
	freeStack(PluginHost::MASTER_IN, mutex);
	for (unsigned i=0; i<channels->size(); i++)
		freeStack(PluginHost::CHANNEL, mutex, channels->at(i));
}





int PluginHost::clonePlugin(Plugin *src, int stackType, pthread_mutex_t *mutex,
  Channel *ch)
{
  juce::PluginDescription pd = src->getPluginDescription();
	Plugin *p = addPlugin(pd.fileOrIdentifier.toStdString(), stackType, mutex, ch);
	if (!p) {
		gLog("[PluginHost::clonePlugin] unable to add new plugin to stack!\n");
		return 0;
	}
	for (int k=0; k<src->getNumParameters(); k++) {
		p->setParameter(k, src->getParameter(k));
	}
	return 1;
}





void PluginHost::processStackOffline(float *buffer, int stackType, Channel *ch, int size)
{
#if 0
	

	

	int index = 0;
	int step  = kernelAudio::realBufsize*2;

	while (index <= size) {
		int left = index+step-size;
		if (left < 0)
			processStack(&buffer[index], stackType, ch);

	

		
		

		index+=step;
	}
#endif
}


#endif 
