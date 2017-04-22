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

def main(args):
    pygame.init()
    screen = pygame.display.set_mode(size)
    clock = pygame.time.Clock()
    
    bar = WidgetBar()
    knob = WidgetKnob()
    fill = 0.0
    
    while 1:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
        
        screen.fill(BACKGROUND_COLOR)
        
        if fill > 1: fill = 0
        
        # draw bar
        bar_off_x = BAR_MARGIN
        bar_off_y = BAR_MARGIN
        bar_width = (WIDTH / BAR_COUNT) - BAR_MARGIN * 2
        bar_height = BAR_HEIGHT
        for i in range(BAR_COUNT-1):
            bar.draw(fill, WidgetBar.Mode.MUTE, screen.subsurface(bar_off_x,bar_off_y,bar_width,bar_height))
            bar_off_x += bar_width + BAR_MARGIN * 2
            
        bar.draw(fill, WidgetBar.Mode.REC, screen.subsurface(bar_off_x,bar_off_y,bar_width,bar_height))
        
        # draw knob
        knob_off_x = BAR_MARGIN
        knob_off_y = BAR_MARGIN + bar_height
        knob_width = bar_width
        knob_height = (HEIGHT-2*BAR_MARGIN) - bar_height
        for i in range(BAR_COUNT):
            knob.draw(fill,screen.subsurface(knob_off_x,knob_off_y,knob_width,knob_height))
            knob_off_x += knob_width + BAR_MARGIN * 2
            
        fill = fill + 0.01
        
        pygame.display.flip()
        
        clock.tick(25)
    
    
    

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
