import enum
import pygame

BORDER_WIDTH_MULT = 4
BORDER_COLOR_REC = (255,0,0)
BORDER_COLOR_PLAY = (0,255,0)
BORDER_COLOR_MUTE = (128,128,128)
BEATS = 4

class WidgetBar:
    
    class Mode(enum.Enum):
        MUTE = 0
        PLAY = 1
        WAIT = 2
        REC = 3
    
    def draw(self, fill, mode, border_width, screen):
        
        x = 0
        y = 0
        w = screen.get_width()
        h = screen.get_height()
        
        if mode is self.Mode.PLAY:
            border_color = BORDER_COLOR_PLAY

        elif mode is self.Mode.REC:
            border_color = BORDER_COLOR_REC

        else:
            border_color = BORDER_COLOR_MUTE
            
        dim_color = list(map(lambda x: x*0.5, border_color))
        dark_color = list(map(lambda x: x*0.25, border_color))
            
        grid_height = h / BEATS
        bar_height_empty = (1.0 - fill) * h
        bar_height_filled = fill * h
        
        # grid fill
        pygame.draw.rect(screen,dim_color,(x,y + (bar_height_empty // grid_height) * grid_height,w,(bar_height_filled // grid_height + 1) * grid_height),0)
        
        # continuos fill
        pygame.draw.rect(screen,border_color,(x,y + bar_height_empty,w,bar_height_filled),0)
        
        # grid
        grid_y = y + grid_height
        for i in range(BEATS-1):
            pygame.draw.line(screen,dim_color,(x,grid_y),(x+w,grid_y),border_width)
            grid_y += grid_height
        
        # frame
        pygame.draw.rect(screen,dim_color,(x,y,w,h),border_width * BORDER_WIDTH_MULT)

