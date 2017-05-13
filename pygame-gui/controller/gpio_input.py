import RPi.GPIO as GPIO  
GPIO.setmode(GPIO.BOARD)
from controller.controller_main import Input
from controller.controller_main import Command

DEBOUNCE = 200
CH0_M = 36
CH0_R = 37
CH1_M = 33
CH1_R = 32
CH2_M = 29
CH2_R = 31
CH3_M = 12
CH3_R = 8

class InputGPIO(Input):
    
    def setup(self):
        self.running = False
    
    def stop(self):
        GPIO.cleanup()
    
    def start(self):
        
        if self.running:
            return
        self.running = True
        
        self.setup_pin(CH0_M,lambda _:self.send_command(Command.MUTE,0))
        self.setup_pin(CH0_R,lambda _:self.send_command(Command.REC,0))
        self.setup_pin(CH1_M,lambda _:self.send_command(Command.MUTE,1))
        self.setup_pin(CH1_R,lambda _:self.send_command(Command.REC,1))
        self.setup_pin(CH2_M,lambda _:self.send_command(Command.MUTE,2))
        self.setup_pin(CH2_R,lambda _:self.send_command(Command.REC,2))
        self.setup_pin(CH3_M,lambda _:self.send_command(Command.MUTE,3))
        self.setup_pin(CH3_R,lambda _:self.send_command(Command.REC,3))
        
    
    def setup_pin(self,pin, callback):
        GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(pin, GPIO.FALLING, callback=callback, bouncetime=DEBOUNCE) 
