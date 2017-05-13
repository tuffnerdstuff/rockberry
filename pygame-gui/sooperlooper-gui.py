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
from controller.controller_main import Controller
from controller.gpio_input import InputGPIO


sl = None
gui = None
inp = None

         
def handle_exit(signal=None,frame=None):
    print("Quitting ...")
    gui.stop()
    inp.stop()
    sl.stop()
    print("Exiting ...")
    sys.exit(0)

def run_as_thread(function):
    t = threading.Thread(target=function)
    t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
    t.start()

if __name__ == '__main__':
    try:
        
        signal.signal(signal.SIGINT, handle_exit)
        
        # Start SL OSC client (new thread)
        sl = SLClient(9951, 9953, "footgui")
        run_as_thread(sl.start)
        
        # Start Controller (new thread)
        cont = Controller(sl)
        inp = InputGPIO(cont)
        run_as_thread(inp.start)
        
        # Start GUI (same thread = blocking)
        gui = Gui(sl)
        gui.start()
        
        print("GUI: Done")
    except KeyboardInterrupt:
        pass
    
    handle_exit()
