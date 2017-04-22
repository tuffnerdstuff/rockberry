import enum
import pygame

BAR_BORDER = 4
BORDER_COLOR_MUTE = (128,128,128)
CELL_FILL_COLOR = (0,0,255)
CELL_BACK_COLOR = (0,0,64)

class WidgetBar:
    
    class Mode(enum.Enum):
        MUTE = 0
        PLAY = 1
        WAIT = 2
        REC = 3
    
    def draw(self, x, y, w, h, fill, mode, screen):
    
        border_width = BAR_BORDER
        
        if mode is self.Mode.PLAY:
            border_color = (0,255,0)

        elif mode is self.Mode.REC:
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
