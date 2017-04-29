import enum
import pygame

BORDER_WIDTH = 3
BORDER_COLOR = (128,128,128)
FILL_COLOR = (0,0,255)
THRESH_COLOR = (255,255,255)
THRESH_COVER_COLOR = (0,0,0,190)
MARGIN_RATIO = 0

class WidgetMeter:
    
    
    def draw(self, volume, threshold, screen):
        
        x = 0
        y = 0
        w = screen.get_width()
        h = screen.get_height()
            
        volume_height_empty = (1.0 - volume) * h
        volume_height_value = volume * h
        threshold_height_empty = (1.0 - threshold) * h
        threshold_height_value = threshold * h
        
        # Volume bar
        pygame.draw.rect(screen,FILL_COLOR,(x+w*MARGIN_RATIO,y + volume_height_empty,w*(1.0-MARGIN_RATIO*2),volume_height_value),0)
        
        # Theshold
        if threshold > 0:
            # threshold dimmer
            s_cover = pygame.Surface((w,threshold_height_value), pygame.SRCALPHA)
            s_cover.fill(THRESH_COVER_COLOR)
            screen.blit(s_cover, (x,y + threshold_height_empty))
            # indicator
            pygame.draw.line(screen,THRESH_COLOR,(x,y + threshold_height_empty),(w,y + threshold_height_empty),BORDER_WIDTH)
            #pygame.draw.rect(screen,THRESH_COVER_COLOR,(x,y + threshold_height_empty,w,threshold_height_empty),0)
        
        # frame
        pygame.draw.rect(screen,BORDER_COLOR,(x,y,w,h),BORDER_WIDTH)
