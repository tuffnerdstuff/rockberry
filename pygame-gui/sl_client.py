#!/usr/bin/env python3
import sys, signal, threading, time, enum
from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import osc_message_builder
from pythonosc import udp_client

IP = "127.0.0.1"
PING_DELAY = 2

class SLClient:
    
    def __init__(self, sl_port, server_port, root):
        self.osc_client = udp_client.UDPClient(IP, sl_port)
        self.osc_server_port = server_port
        self.osc_dispatcher = fromsl_dispatcher = dispatcher.Dispatcher()
        self.osc_server = osc_server.ThreadingOSCUDPServer((IP, self.osc_server_port), self.osc_dispatcher)
        self.osc_server_root = root
        
        self.last_update_time = 0
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
        
        
        # unregister SL loop count update (same info as ping)
        msg = osc_message_builder.OscMessageBuilder(address = "/unregister")
        msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
        msg.add_arg("/{}/sl".format(self.osc_server_root))
        msg = msg.build()
        self.osc_client.send(msg)
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
        if fun == "ping" or fun == "sl":
            
            hosturl = args[0]
            version = args[1]
            loop_count = args[2]
            
            if not self.sl:
                self.sl = SooperLooper()
            
            # Lazily adding additional loops if necessary
            if loop_count > self.sl.loop_count:
                for l in range(loop_count - self.sl.loop_count):
                    self.sl.loops.append(Loop())
            
            # Set SL attributes
            self.sl.hosturl = hosturl
            self.sl.version = version
            self.sl.loop_count = int(loop_count)
            
            # If last ping answer is longer than 2*PING_DELAY ago, re-request auto updates
            if fun == "sl" or self._get_millis() - self.last_update_time > 2*PING_DELAY*1000:
                print("--> Registering for updates")
                # Request SL loop count update (same info as ping)
                msg = osc_message_builder.OscMessageBuilder(address = "/register")
                msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
                msg.add_arg("/{}/sl".format(self.osc_server_root))
                msg = msg.build()
                self.osc_client.send(msg)
                
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
            
            # update time
            self.last_update_time = self._get_millis() 
            
        elif fun == "loop":
            loop_nr = int(args[0])
            ctrl = args[1]
            value = args[2]
            
            loop = self.sl.loops[loop_nr]
            
            if ctrl == "loop_len":
                loop.length = float(value)
            elif ctrl == "loop_pos":
                loop.pos = float(value)
            elif ctrl == "rec_thresh":
                loop.threshold = float(value)
            elif ctrl == "rate_input":
                loop.rate_input = float(value)
            elif ctrl == "rate_output":
                loop.rate_output = float(value)
            
           
            

        
    def ping_sl(self):
        while self.running:
            print("OUT:Ping...")
            # Ping
            ping_msg = osc_message_builder.OscMessageBuilder(address = "/ping")
            ping_msg.add_arg("osc.udp://{}:{}".format(IP,self.osc_server_port))
            ping_msg.add_arg("/{}/ping".format(self.osc_server_root))
            ping_msg = ping_msg.build()
            self.osc_client.send(ping_msg)
            time.sleep(PING_DELAY)
            
    def _get_millis(self):
        return time.time() * 1000
            

class SooperLooper:
    def __init__(self):
        self.hosturl = "" 
        self.version = ""
        self.loop_count = 0
        self.loops = []
    

class Loop:
    
    ATTRS = ("loop_len","loop_pos","state","cycle_len","free_time","total_time","rate_output","in_peak_meter","out_peak_meter","is_soloed","rec_thresh")
    
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
        self.length = 0.0
        self.pos = 0.0
        self.threshold = 0.0
        self.rate_input = 0.0
        self.rate_output = 0.0
        self.mode = self.Mode.UNKNOWN
        
    def get_relative_pos(self):
        if self.length > 0:
            return self.pos / self.length
        else:
            return 0.0
        

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
