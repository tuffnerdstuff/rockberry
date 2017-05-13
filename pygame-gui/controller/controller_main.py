import enum

class Controller:
    
    def __init__(self,sl_client):
        self.sl_client = sl_client
        
    def input_callback(self,command,loop=0):
        print("CALLBACK: command={} loop={}".format(command,loop))
        
        if self.sl_client and self.sl_client.get_sl().loop_count > 0:
            if command == Command.REC:
                print("REC loop={}".format(loop))
                self.sl_client.get_sl().get_loop(loop).record()
            elif command == Command.MUTE:
                print("MUTE loop={}".format(loop))
                self.sl_client.get_sl().get_loop(loop).mute()
            elif command == Command.REC_MODE:
                print("REC_MODE")
                pass
            elif command == Command.MUTE_ALL:
                print("MUTE_ALL")
                pass    

class Command(enum.IntEnum):
    REC = 0
    MUTE = 1
    REC_MODE = 2
    MUTE_ALL = 3

class Input:
    def __init__(self,controller):
        self.controller = controller
        self.setup()
    
    def setup(self):
        pass # override if you need to do setup stuff
        
    def send_command(self,command,loop=0):
        if self.controller:
            self.controller.input_callback(command,loop)
        
if __name__ == '__main__':
    import controller.key_input
    c = Controller(None)
    i = key_input.InputKey(c)
    i.start()
    
