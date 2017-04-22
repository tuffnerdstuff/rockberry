#!/usr/bin/env python3
import pygame

pygame.init()
s1 = pygame.display.set_mode((720, 576))
s2 = s1.subsurface(s1.get_width()/2,s1.get_height()/2,s1.get_width()/2,s1.get_height()/2)
s3 = s2.subsurface(s2.get_width()/2,s2.get_height()/2,s2.get_width()/2,s2.get_height()/2)
clock = pygame.time.Clock()

while 1:
    s1.fill((255,0,0))
    s2.fill((0,255,0))
    s3.fill((0,0,255))
    
    pygame.display.flip()
    clock.tick(25)
