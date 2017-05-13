from controller.controller_main import Input
from controller.controller_main import Command
from Xlib.display import Display
from Xlib import X

key_a = 38 # loop 0 REC
key_y = 52 # loop 0 MUTE
key_s = 39 # loop 1 REC
key_x = 53 # loop 1 MUTE
key_d = 40 # loop 2 REC
key_c = 54 # loop 2 MUTE
key_f = 41 # loop 3 REC
key_v = 55 # loop 3 MUTE
key_1 = 10 # global REC MODE
key_q = 24 # global MUTE ALL

keys = [key_a,key_y,key_s,key_x,key_d,key_c,key_f,key_v,key_1,key_q]

class InputKey(Input):
    
    def stop(self):
        self.running = False
    
    def start(self):
        
        if self.running:
            return 
        
        self.running = True
        
        # current display
        disp = Display()
        root = disp.screen().root

        # we tell the X server we want to catch keyPress event
        root.change_attributes(event_mask = X.KeyPressMask)

        for keycode in keys:
            root.grab_key(keycode, X.AnyModifier, 1,X.GrabModeAsync, X.GrabModeAsync)

        while self.running:
            event = root.display.next_event()
            self.handle_event(event)
        

    def handle_event(self,aEvent):
        keycode = aEvent.detail
        if aEvent.type == X.KeyPress:
            if keycode == key_a:
                print("A")
                self.send_command(Command.REC,0)
            elif keycode == key_y:
                print("Y")
                self.send_command(Command.MUTE,0)
            elif keycode == key_s:
                print("S")
                self.send_command(Command.REC,1)
            elif keycode == key_x:
                print("X")
                self.send_command(Command.MUTE,1)
            elif keycode == key_d:
                print("D")
                self.send_command(Command.REC,2)
            elif keycode == key_c:
                print("C")
                self.send_command(Command.MUTE,2)
            elif keycode == key_f:
                print("F")
                self.send_command(Command.REC,3)
            elif keycode == key_v:
                print("V")
                self.send_command(Command.MUTE,3)
            elif keycode == key_1:
                print("1")
                self.send_command(Command.REC_MODE)
            elif keycode == key_q:
                print("Q")
                self.send_command(Command.MUTE_ALL)
                
    

