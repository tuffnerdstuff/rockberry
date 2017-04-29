import pygame, math

BORDER_COLOR = (128,128,128)
BORDER_WIDTH = 3
OFFSET = 0.375
BANDWIDTH = 0.75
class WidgetKnob:
    def draw(self,value,surface):
        
        w = surface.get_width()
        h = surface.get_height()
        r = int(min(w, h) / 2) - BORDER_WIDTH
        x_center = int(w / 2)
        y_center = int(h / 2)
        rad = (OFFSET + value * BANDWIDTH) * (2*math.pi)
        x_border = x_center + (r - BORDER_WIDTH*2) * math.cos(rad)
        y_border = y_center + (r - BORDER_WIDTH*2) * math.sin(rad)
        
        # border
        pygame.draw.circle(surface, BORDER_COLOR, (x_center,y_center), (r - BORDER_WIDTH*2), BORDER_WIDTH)
        
        # indicator
        pygame.draw.line(surface, BORDER_COLOR, (x_center,y_center),(x_border,y_border), BORDER_WIDTH)
        
        # units
        unit_off = 0
        for i in range(11):
            rad_unit = (OFFSET + unit_off * BANDWIDTH) * (2*math.pi)
            x_unit = int(x_center + r * math.cos(rad_unit))
            y_unit = int(y_center + r * math.sin(rad_unit))
            pygame.draw.circle(surface, BORDER_COLOR, (x_unit,y_unit), BORDER_WIDTH)
            unit_off += 0.10
            
        
