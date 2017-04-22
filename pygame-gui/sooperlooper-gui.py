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

import pygame
from widgets.bar import WidgetBar 
from widgets.knob import WidgetKnob

size = WIDTH, HEIGHT = 720, 576
BACKGROUND_COLOR = (0,0,0)

BAR_MARGIN = 5
BAR_HEIGHT = (HEIGHT-2*BAR_MARGIN) * 0.75
BAR_COUNT = 4

fill = 0.0

def main(args):
    pygame.init()
    s_screen = pygame.display.set_mode(size)
    s_content = s_screen.subsurface(BAR_MARGIN,BAR_MARGIN,s_screen.get_width()-2*BAR_MARGIN,s_screen.get_height()-2*BAR_MARGIN)
    clock = pygame.time.Clock()
    
    bar = WidgetBar()
    s_bar = s_content.subsurface(0,0,s_content.get_width(),s_content.get_height()*0.75)
    knob = WidgetKnob()
    s_knob = s_content.subsurface(0,s_bar.get_height()+BAR_MARGIN,s_content.get_width(),s_content.get_height() - (s_bar.get_height()+BAR_MARGIN))
    
    global fill
    
    while 1:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
        
        s_screen.fill(BACKGROUND_COLOR)
        
        if fill > 1: fill = 0
        
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
        width = (surface.get_width() / BAR_COUNT) - BAR_MARGIN + BAR_MARGIN / BAR_COUNT
        height = surface.get_height()
        for i in range(BAR_COUNT-1):
            widget.draw(fill, WidgetBar.Mode.MUTE, surface.subsurface(x,y,width,height))
            x += width + BAR_MARGIN
            
        widget.draw(fill, WidgetBar.Mode.REC, surface.subsurface(x,y,width,height))

def draw_knobs(dummy, surface, widget):
    if dummy:
        surface.fill((0,255,0))
    else:
        x = 0
        y = 0
        width = (surface.get_width() / BAR_COUNT) - BAR_MARGIN + BAR_MARGIN / BAR_COUNT
        height = surface.get_height()
        for i in range(BAR_COUNT):
            widget.draw(fill, surface.subsurface(x,y,width,height))
            x += width + BAR_MARGIN
            
if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
