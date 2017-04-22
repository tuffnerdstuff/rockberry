import pygame, math

BORDER_COLOR = (128,128,128)
BORDER_WIDTH = 5

class WidgetKnob:
    def draw(self,value,surface):
        
        w = surface.get_width()
        h = surface.get_height()
        r = int(min(w, h) / 2)
        x_center = int(w / 2)
        y_center = int(h / 2)
        rad = value * (2*math.pi)
        x_border = x_center + r * math.cos(rad)
        y_border = y_center + r * math.sin(rad)
        
        # border
        pygame.draw.circle(surface, BORDER_COLOR, (x_center,y_center), r, BORDER_WIDTH)
        
        # indicator
        pygame.draw.line(surface, BORDER_COLOR, (x_center,y_center),(x_border,y_border), BORDER_WIDTH)
