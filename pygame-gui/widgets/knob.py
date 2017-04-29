import pygame, math

BORDER_COLOR = (128,128,128)
OFFSET = 0.375
BANDWIDTH = 0.75
class WidgetKnob:
    def draw(self,value,border_width,surface):
        
        w = surface.get_width()
        h = surface.get_height()
        r = int(min(w, h) / 2) - border_width
        x_center = int(w / 2)
        y_center = int(h / 2)
        rad = (OFFSET + value * BANDWIDTH) * (2*math.pi)
        x_border = x_center + (r - border_width*2) * math.cos(rad)
        y_border = y_center + (r - border_width*2) * math.sin(rad)
        
        # border
        pygame.draw.circle(surface, BORDER_COLOR, (x_center,y_center), (r - border_width*2), border_width)
        
        # indicator
        pygame.draw.line(surface, BORDER_COLOR, (x_center,y_center),(x_border,y_border), border_width)
        
        # units
        unit_off = 0
        for i in range(11):
            rad_unit = (OFFSET + unit_off * BANDWIDTH) * (2*math.pi)
            x_unit = int(x_center + r * math.cos(rad_unit))
            y_unit = int(y_center + r * math.sin(rad_unit))
            pygame.draw.circle(surface, BORDER_COLOR, (x_unit,y_unit), border_width)
            unit_off += 0.10
            
        
