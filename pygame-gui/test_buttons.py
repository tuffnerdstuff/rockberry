#!/usr/bin/env python3

import sys
import RPi.GPIO as GPIO  
from time import sleep

GPIO.setmode(GPIO.BOARD)

i = 0
j = 0
p = 0
PIN = 37
BOUNCETIME = 300


def setup_pin(pin, callback):
    GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.add_event_detect(pin, GPIO.FALLING, callback=callback, bouncetime=BOUNCETIME) 

def bark(wut):
    global i
    i = i + 1
    sleep(0.005)
    if GPIO.input(PIN) == 0:
        global j
        j = j + 1 

GPIO.setmode(GPIO.BOARD)
setup_pin(PIN,bark)
while(True):
    if not GPIO.input(PIN): p = p + 1
    sys.stdout.write("SIGNALS:%i EVENTS:%i EVENTS_FILTERED:%i\r" % (p,i,j))
    sys.stdout.flush() 
