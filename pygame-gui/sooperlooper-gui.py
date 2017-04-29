#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  sooperlooper-gui.py
#  
#  Copyright 2017 Stefan <stefan@pingubook>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  

import pygame, sl_client, threading, signal, sys
from widgets.bar import WidgetBar 
from widgets.knob import WidgetKnob
from widgets.meter import WidgetMeter

size = WIDTH, HEIGHT = 720, 576
BACKGROUND_COLOR = (0,0,0)
BAR_MARGIN = 5
BAR_HEIGHT = (HEIGHT-2*BAR_MARGIN) * 0.75
BAR_COUNT = 4

sl = None

gui_running = True
fill = 0.0

def main():
    pygame.init()
    s_screen = pygame.display.set_mode(size, pygame.SRCALPHA|pygame.HWSURFACE|pygame.FULLSCREEN)
    s_content = s_screen.subsurface(BAR_MARGIN,BAR_MARGIN,s_screen.get_width()-2*BAR_MARGIN,s_screen.get_height()-2*BAR_MARGIN)
    clock = pygame.time.Clock()
    
    meter = WidgetMeter()
    s_meter = s_content.subsurface(0,0,s_content.get_width()*0.025,s_content.get_height()*0.75)
    s_meter_out = s_content.subsurface(s_content.get_width()-s_meter.get_width(),0,s_content.get_width()*0.025,s_content.get_height()*0.75)
    
    s_content_loops = s_content.subsurface(s_meter.get_width()+BAR_MARGIN,0,s_content.get_width()-(s_meter.get_width()+s_meter_out.get_width()+BAR_MARGIN*2),s_content.get_height())
    
    bar = WidgetBar()
    s_bar = s_content_loops.subsurface(0,0,s_content_loops.get_width(),s_content_loops.get_height()*0.75)
    
    knob = WidgetKnob()
    s_knob = s_content_loops.subsurface(0,s_bar.get_height()+BAR_MARGIN,s_content_loops.get_width(),s_content_loops.get_height() - (s_bar.get_height()+BAR_MARGIN))
    
    global fill
    
    while gui_running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
        
        s_screen.fill(BACKGROUND_COLOR)
        
        if fill > 1: fill = 0
        
        if sl.sl and sl.sl.loop_count > 0:
        
            # draw volume meter
            #draw_meter(False, s_meter, meter, sl.sl.loops[0].rate_input, sl.sl.loops[0].threshold)
            draw_meter(False, s_meter, meter, fill, sl.sl.loops[0].threshold)
            draw_meter(False, s_meter_out, meter, fill, -1)
        
            # draw bars
            draw_bars(False, s_bar, bar)
            
            # draw knob
            draw_knobs(False, s_knob, knob)
            
        fill = fill + 0.01
        
        pygame.display.flip()
        
        clock.tick(25)
    
def draw_bars(dummy, surface, widget):
    if dummy:
        surface.fill((255,0,0))
    else:
        x = 0
        y = 0
        width = (surface.get_width() / sl.sl.loop_count) - BAR_MARGIN + BAR_MARGIN / sl.sl.loop_count
        height = surface.get_height()
        for i in range(sl.sl.loop_count):
            widget.draw(sl.sl.loops[i].get_relative_pos(), WidgetBar.Mode.MUTE, surface.subsurface(x,y,width,height))
            x += width + BAR_MARGIN
            

def draw_knobs(dummy, surface, widget):
    if dummy:
        surface.fill((0,255,0))
    else:
        x = 0
        y = 0
        width = (surface.get_width() / sl.sl.loop_count) - BAR_MARGIN + BAR_MARGIN / sl.sl.loop_count
        height = surface.get_height()
        for i in range(sl.sl.loop_count):
            widget.draw(sl.sl.loops[i].threshold, surface.subsurface(x,y,width,height))
            x += width + BAR_MARGIN
            
def draw_meter(dummy, surface, widget, volume, threshold):
    if dummy:
        surface.fill((0,0,255))
    else:
        x = 0
        y = 0
        width = surface.get_width()
        height = surface.get_height()
        widget.draw(volume, threshold, surface.subsurface(x,y,width,height))
            
def handle_exit(signal=None,frame=None):
    print("Quitting ...")
    sl.stop()
    gui_running = False
    sys.exit(0)

if __name__ == '__main__':
    try:
        signal.signal(signal.SIGINT, handle_exit)
        
        # Start SL OSC client
        sl = sl_client.SLClient(9951, 9952, "footgui")
        t = threading.Thread(target=sl.start)
        t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
        t.start()
        
        # Start GUI
        main()
    except KeyboardInterrupt:
        handle_exit()
