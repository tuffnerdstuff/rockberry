#!/usr/bin/env python3
import sys, signal, threading, time, enum
from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import osc_message_builder
from pythonosc import udp_client

IP = "127.0.0.1"
PING_DELAY = 10

class SLClient:
    
    def __init__(self, sl_port, server_port, root):
        self.osc_client = udp_client.UDPClient(IP, sl_port)
        self.osc_server_port = server_port
        self.osc_dispatcher = fromsl_dispatcher = dispatcher.Dispatcher()
        self.osc_server = osc_server.ThreadingOSCUDPServer((IP, self.osc_server_port), self.osc_dispatcher)
        self.osc_server_root = root
        
        self.sl = None
    
    def start(self):
        
        self.running = True
                    
        ##########
        # CLIENT #
        ##########
        # Ping SL for state updates
        t = threading.Thread(target=self.ping_sl)
        t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
        t.start()
        
        
        ##########
        # SERVER #
        ##########
        
        # Dispatcher: listen to messages to "/<osc_server_root>"
        self.osc_dispatcher.map("/{}/*".format(self.osc_server_root), self.parse_osc_message)
        # Server: start (serve_forever() blocks)
        self.osc_server.serve_forever()
        
    def stop(self):
        self.running = False
        
        # unregister loop auto updates
        if self.sl:
            for loop in range(self.sl.loop_count):
                    for attr in Loop.ATTRS:
                        msg = osc_message_builder.OscMessageBuilder(address = "/sl/{}/unregister_auto_update".format(loop))
                        msg.add_arg(attr)
                        msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
                        msg.add_arg("/{}/loop".format(self.osc_server_root))
                        msg = msg.build()
                        self.osc_client.send(msg)
            
        
    def parse_osc_message(self,unused_addr, *args):
        print("IN:{} {}".format(unused_addr,args))
        
        fun = unused_addr.split("/")[-1]
        if fun == "global":
            
            hosturl = args[0]
            version = args[1]
            loop_count = args[2]
            
            if not self.sl:
                self.sl = SooperLooper()
            
            # Add/remove loops
            if loop_count != self.sl.loop_count:
                self.sl.loops = []
                for l in range(self.sl.loop_count):
                    self.sl.loops.append(Loop())
            
            # Set SL attributes
            self.sl.hosturl = hosturl
            self.sl.version = version
            self.sl.loop_count = int(loop_count)
            
            # Request loop updates
            for loop in range(self.sl.loop_count):
                for attr in Loop.ATTRS:
                    
                    # Get current loop state
                    msg = osc_message_builder.OscMessageBuilder(address = "/sl/{}/get".format(loop))
                    msg.add_arg(attr)
                    msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
                    msg.add_arg("/{}/loop".format(self.osc_server_root))
                    msg = msg.build()
                    self.osc_client.send(msg)
                    
                    # Register for loop state auto-updates (only triggered when loop actually changes)
                    msg = osc_message_builder.OscMessageBuilder(address = "/sl/{}/register_auto_update".format(loop))
                    msg.add_arg(attr)
                    msg.add_arg(100)
                    msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
                    msg.add_arg("/{}/loop".format(self.osc_server_root))
                    msg = msg.build()
                    self.osc_client.send(msg)
                    #print("get attr:{} loop:{}".format(attr,loop))

        
    def ping_sl(self):
        while self.running:
            # Ping
            ping_msg = osc_message_builder.OscMessageBuilder(address = "/ping")
            ping_msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
            ping_msg.add_arg("/{}/global".format(self.osc_server_root))
            ping_msg = ping_msg.build()
            self.osc_client.send(ping_msg)
            time.sleep(PING_DELAY)
            

class SooperLooper:
    def __init__(self):
        self.hosturl = "" 
        self.version = ""
        self.loop_count = 0
        self.loops = []
    

class Loop:
    
    ATTRS = ("loop_len","loop_pos","state","cycle_len","free_time","total_time","rate_output","in_peak_meter","out_peak_meter","is_soloed")
    
    class Mode(enum.Enum):
        UNKNOWN = -1
        OFF = 0
        WAIT_START = 1
        RECORDING = 2
        WAIT_STOP = 3
        PLAYING = 4
        OVERDUBBING = 5
        MULTIPLYING = 6
        INSERTING = 7
        REPLACING = 8
        DELAY = 9
        MUTED_PLAYING = 10
        SCRATCHING = 11
        ONESHOT = 12
        SUBSTITUTE = 13
        PAUSED = 14
        MUTED_STOP = 20
    
    def __init__(self):
        self.pos = 0.0
        self.mode = self.Mode.UNKNOWN
        

def handle_exit(signal=None,frame=None):
    print("Quitting ...")
    c.stop()
    sys.exit(0)

if __name__ == '__main__':
    try:
        signal.signal(signal.SIGINT, handle_exit)
        
        c = SLClient(9951, 9952, "mygui")
        c.start()
    except KeyboardInterrupt:
            handle_exit()
