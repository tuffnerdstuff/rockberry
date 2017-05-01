#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  sooperlooper-gui.py
#  
#  Copyright 2017 Stefan <stefan@pingubook>
#  
#  

import pygame, sl_client, threading, signal, sys
from widgets.bar import WidgetBar 
from widgets.knob import WidgetKnob
from widgets.meter import WidgetMeter

size = WIDTH, HEIGHT = 1024, 576
#size = WIDTH, HEIGHT = 300, 200
BACKGROUND_COLOR = (0,0,0)
MARGIN = int(WIDTH * 0.008)
BORDER = int(WIDTH * 0.004)

sl = None

gui_running = True
fill = 0.0

def main():
    pygame.init()
    s_screen = pygame.display.set_mode(size, pygame.SRCALPHA|pygame.HWSURFACE|pygame.FULLSCREEN)
    #s_screen = pygame.display.set_mode(size, pygame.SRCALPHA|pygame.HWSURFACE)
    s_content = s_screen.subsurface(MARGIN,MARGIN,s_screen.get_width()-2*MARGIN,s_screen.get_height()-2*MARGIN)
    pygame.mouse.set_visible(False)
    clock = pygame.time.Clock()
    
    meter = WidgetMeter()
    s_meter = s_content.subsurface(0,0,s_content.get_width()*0.025,s_content.get_height()*0.75)
    s_meter_out = s_content.subsurface(s_content.get_width()-s_meter.get_width(),0,s_content.get_width()*0.025,s_content.get_height()*0.75)
    
    s_content_loops = s_content.subsurface(s_meter.get_width()+MARGIN,0,s_content.get_width()-(s_meter.get_width()+s_meter_out.get_width()+MARGIN*2),s_content.get_height())
    
    bar = WidgetBar()
    s_bar = s_content_loops.subsurface(0,0,s_content_loops.get_width(),s_content_loops.get_height()*0.75)
    
    knob = WidgetKnob()
    s_knob = s_content_loops.subsurface(0,s_bar.get_height()+MARGIN,s_content_loops.get_width(),s_content_loops.get_height() - (s_bar.get_height()+MARGIN))
    
    global fill, gui_running
    
    while gui_running:
        for event in pygame.event.get():
            
            sprlpr = sl.get_sl()
            
            # Closing pygame window
            if event.type == pygame.QUIT:
                print("EVENT: QUIT") 
                gui_running = False
            # Key Press Handling
            elif event.type == pygame.KEYDOWN:
                # Quit
                if event.key == pygame.K_ESCAPE:
                    print("KEY: ESCAPE")
                    gui_running = False
                elif not sprlpr or sprlpr.loop_count < 1:
                    print("KEY: blocked, no connection/loops!")
                # Mute/Unmute 1
                elif event.key == pygame.K_y:
                    print("KEY: Y")
                    sprlpr.get_loop(0).mute()
                # Record 1
                elif event.key == pygame.K_a:
                    print("KEY: A")
                    sprlpr.get_loop(0).record()
                elif sprlpr.loop_count < 2:
                    print("KEY: blocked, less than 2 loops!")
                # Mute/Unmute 2
                elif event.key == pygame.K_x:
                    print("KEY: X")
                    sprlpr.get_loop(1).mute()
                # Record 2
                elif event.key == pygame.K_s:
                    print("KEY: S")
                    sprlpr.get_loop(1).record()
                elif sprlpr.loop_count < 3:
                    print("KEY: blocked, less than 3 loops!")
                # Mute/Unmute 3
                elif event.key == pygame.K_c:
                    print("KEY: C")
                    sprlpr.get_loop(2).mute()
                # Record 3
                elif event.key == pygame.K_d:
                    print("KEY: D")
                    sprlpr.get_loop(2).record()
                elif sprlpr.loop_count < 4:
                    print("KEY: blocked, less than 4 loops!")
                # Mute/Unmute 4
                elif event.key == pygame.K_v:
                    print("KEY: V")
                    sprlpr.get_loop(3).mute()
                # Record 4
                elif event.key == pygame.K_f:
                    print("KEY: F")
                    sprlpr.get_loop(3).record()

        
        s_screen.fill(BACKGROUND_COLOR)
        
        if fill > 1: fill = 0
        
        if sl.get_sl() and sl.get_sl().loop_count > 0:
        
            # draw volume meter
            #draw_meter(False, s_meter, meter, sl.get_sl().loops[0].rate_input, sl.get_sl().loops[0].threshold)
            draw_meter(False, s_meter, meter, fill, sl.get_sl().get_loop(0).threshold)
            draw_meter(False, s_meter_out, meter, fill, -1)
        
            # draw bars
            draw_bars(False, s_bar, bar)
            
            # draw knob
            draw_knobs(False, s_knob, knob)
            
        fill = fill + 0.01
        
        pygame.display.flip()
        
        clock.tick(60)
    
def draw_bars(dummy, surface, widget):
    if dummy:
        surface.fill((255,0,0))
    else:
        x = 0
        y = 0
        width = (surface.get_width() / sl.get_sl().loop_count) - MARGIN + MARGIN / sl.get_sl().loop_count
        height = surface.get_height()
        for i in range(sl.get_sl().loop_count):
            loop = sl.get_sl().get_loop(i)
            widget.draw(loop.get_relative_pos(), get_bar_state(loop.state), BORDER, surface.subsurface(x,y,width,height))
            x += width + MARGIN
            
def get_bar_state(sl_state):
    if sl_state == sl_client.Loop.State.WAIT_START or sl_state == sl_client.Loop.State.WAIT_STOP:
        return WidgetBar.Mode.WAIT
    elif sl_state == sl_client.Loop.State.PLAYING:
        return WidgetBar.Mode.PLAY
    elif sl_state == sl_client.Loop.State.RECORDING:
        return WidgetBar.Mode.REC
    else:
        return WidgetBar.Mode.MUTE

def draw_knobs(dummy, surface, widget):
    if dummy:
        surface.fill((0,255,0))
    else:
        x = 0
        y = 0
        width = (surface.get_width() / sl.get_sl().loop_count) - MARGIN + MARGIN / sl.get_sl().loop_count
        height = surface.get_height()
        for i in range(sl.get_sl().loop_count):
            widget.draw(sl.get_sl().get_loop(i).threshold, BORDER, surface.subsurface(x,y,width,height))
            x += width + MARGIN
            
def draw_meter(dummy, surface, widget, volume, threshold):
    if dummy:
        surface.fill((0,0,255))
    else:
        x = 0
        y = 0
        width = surface.get_width()
        height = surface.get_height()
        widget.draw(volume, threshold, BORDER, surface.subsurface(x,y,width,height))
            
def handle_exit(signal=None,frame=None):
    print("Quitting ...")
    sl.stop()
    global gui_running
    gui_running = False
    sys.exit(0)

if __name__ == '__main__':
    try:
        signal.signal(signal.SIGINT, handle_exit)
        
        # Start SL OSC client
        sl = sl_client.SLClient(9951, 9953, "footgui")
        t = threading.Thread(target=sl.start)
        t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
        t.start()
        
        # Start GUI
        main()
    except KeyboardInterrupt:
        pass
    
    handle_exit()
