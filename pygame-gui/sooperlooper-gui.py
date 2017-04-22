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

import pygame, enum

size = WIDTH, HEIGHT = 720, 576
BACKGROUND_COLOR = (0,0,0)

class Mode(enum.Enum):
    MUTE = 0
    PLAY = 1
    WAIT = 2
    REC = 3

BAR_HEIGHT = HEIGHT
BAR_MARGIN = 5
BAR_COUNT = 4
BAR_BORDER = 4
BORDER_COLOR_MUTE = (128,128,128)
CELL_FILL_COLOR = (0,0,255)
CELL_BACK_COLOR = (0,0,64)

def main(args):
    pygame.init()
    screen = pygame.display.set_mode(size)
    clock = pygame.time.Clock()
    
    fill = 0.0
    
    while 1:
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()
        
        screen.fill(BACKGROUND_COLOR)
        
        # draw bar
        if fill > 1: fill = 0
        
        bar_off_x = BAR_MARGIN
        bar_off_y = BAR_MARGIN
        bar_width = (WIDTH / BAR_COUNT) - BAR_MARGIN * 2
        bar_height = BAR_HEIGHT - BAR_MARGIN * 2
        for i in range(BAR_COUNT-1):
            draw_bar(bar_off_x,bar_off_y,bar_width,bar_height, fill, Mode.MUTE, screen)
            bar_off_x += bar_width + BAR_MARGIN * 2
            
        draw_bar(bar_off_x,bar_off_y,bar_width,bar_height, fill, Mode.REC, screen)
            
        fill = fill + 0.01
        
        pygame.display.flip()
        
        clock.tick(25)
        
def draw_bar(x, y, w, h, fill, mode, screen):
    
    border_width = BAR_BORDER
    
    if mode is Mode.PLAY:
        border_color = (0,255,0)

    elif mode is Mode.REC:
        border_color = (255,0,0)

    else:
        border_color = BORDER_COLOR_MUTE
        
        
    grid_height = h / 4
    bar_height_empty = (1.0 - fill) * h
    bar_height_filled = fill * h
    
    # grid fill
    pygame.draw.rect(screen,CELL_BACK_COLOR,(x,y + (bar_height_empty // grid_height) * grid_height,w,(bar_height_filled // grid_height + 1) * grid_height),0)
    
    # continuos fill
    pygame.draw.rect(screen,CELL_FILL_COLOR,(x,y + bar_height_empty,w,bar_height_filled),0)
    
    # grid
    grid_y = y + grid_height
    for i in range(3):
        pygame.draw.line(screen,BORDER_COLOR_MUTE,(x,grid_y),(x+w,grid_y),BAR_BORDER)
        grid_y += grid_height
    
    # frame
    pygame.draw.rect(screen,border_color,(x,y,w,h),border_width)
    
    
    
    

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
