#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  sooperlooper-gui.py
#  
#  Copyright 2017 Stefan <stefan@pingubook>
#  
#  

import threading, signal, sys
from sl_client.sl_client import SLClient
from gui.gui_main import Gui


sl = None
gui = None

         
def handle_exit(signal=None,frame=None):
    print("Quitting ...")
    sl.stop()
    gui.stop()
    print("Exiting ...")
    sys.exit(0)

if __name__ == '__main__':
    try:
        signal.signal(signal.SIGINT, handle_exit)
        
        # Start SL OSC client
        sl = SLClient(9951, 9953, "footgui")
        t = threading.Thread(target=sl.start)
        t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
        t.start()
        
        # Start GUI
        gui = Gui(sl)
        gui.start()
        
        print("GUI: Done")
    except KeyboardInterrupt:
        pass
    
    handle_exit()
