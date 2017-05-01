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
        self._sl = None
    
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
        self.send_osc_message(
            "/unregister",
            "osc.udp://{}:{}".format(IP,self.osc_server_port),
            "/{}/sl".format(self.osc_server_root)
        )
        
        # unregister loop auto updates
        if self._sl:
            for loop in range(self._sl.loop_count):
                    for attr in Loop.ATTRS:
                        self.send_osc_message(
                            "/sl/{}/unregister_auto_update".format(loop),
                            attr,
                            "osc.udp://{}:{}".format(IP,self.osc_server_port),
                            "/{}/loop".format(self.osc_server_root)
                        )
                        
    def get_sl(self):
        return self._sl
                        
    def send_osc_message(self,address,*args):
        msg = osc_message_builder.OscMessageBuilder(address)
        for arg in args:
            msg.add_arg(arg)
        self.osc_client.send(msg.build())
            
        
    def parse_osc_message(self,unused_addr, *args):
        print("IN:{} {}".format(unused_addr,args))
        
        
        
        fun = unused_addr.split("/")[-1]
        if fun == "ping" or fun == "sl":
            
            hosturl = args[0]
            version = args[1]
            loop_count = args[2]
            
            if not self._sl:
                self._sl = SooperLooper()
            
            # Lazily adding additional loops if necessary
            if loop_count > self._sl.loop_count:
                for l in range(loop_count - self._sl.loop_count):
                    self._sl._loops.append(Loop(self,self._sl.loop_count + l))
            
            # Set SL attributes
            self._sl.hosturl = hosturl
            self._sl.version = version
            self._sl.loop_count = int(loop_count)
            
            # If last ping answer is longer than 2*PING_DELAY ago, re-request auto updates
            if fun == "sl" or get_millis() - self.last_update_time > 2*PING_DELAY*1000:
                print("--> Registering for updates")
                # Request SL loop count update (same info as ping)
                self.send_osc_message(
                    "/register",
                    "osc.udp://{}:{}".format(IP,self.osc_server_port),
                    "/{}/sl".format(self.osc_server_root),
                )
                
                # Request loop updates
                for loop in range(self._sl.loop_count):
                    for attr in Loop.ATTRS:
                        
                        # Get current loop state
                        self.send_osc_message(
                            "/sl/{}/get".format(loop),
                            attr,
                            "osc.udp://{}:{}".format(IP,self.osc_server_port),
                            "/{}/loop".format(self.osc_server_root)
                        )
                        
                        # Register for loop state auto-updates (only triggered when loop actually changes)
                        self.send_osc_message(
                            "/sl/{}/register_auto_update".format(loop),
                            attr,
                            100,
                            "osc.udp://{}:{}".format(IP,self.osc_server_port),
                            "/{}/loop".format(self.osc_server_root)
                        )
            
            # update time
            self.last_update_time = get_millis() 
            
        elif fun == "loop":
            loop_nr = int(args[0])
            ctrl = args[1]
            value = args[2]
            
            loop = self._sl._loops[loop_nr]
            
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
            elif ctrl == "state":
                loop.state = Loop.State(int(value))
            
           
            

        
    def ping_sl(self):
        while self.running:
            print("OUT:Ping...")
            # Ping
            self.send_osc_message(
                "/ping",
                "osc.udp://{}:{}".format(IP,self.osc_server_port),
                "/{}/ping".format(self.osc_server_root)
            )
            time.sleep(PING_DELAY)
            
            

class SooperLooper:
    def __init__(self):
        self.hosturl = "" 
        self.version = ""
        self.loop_count = 0
        self._loops = []
        
    def get_loop(self,index):
        if index >= len(self._loops):
            return None
        else:
            return self._loops[index]
    

class Loop:
    
    ATTRS = ("loop_len","loop_pos","state","cycle_len","free_time","total_time","rate_output","in_peak_meter","out_peak_meter","is_soloed","rec_thresh")
    
    class State(enum.Enum):
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
    
    def __init__(self,client,loop_nr):
        self.client = client
        self.loop_nr = loop_nr
        
        self.length = 0.0
        self.pos = 0.0
        self.threshold = 0.0
        self.rate_input = 0.0
        self.rate_output = 0.0
        self.state = self.State.UNKNOWN
    
    def get_relative_pos(self):
        if self.length > 0:
            return self.pos / self.length
        else:
            return 0.0
    
    def record(self):
        self.client.send_osc_message(
            "/sl/{}/hit".format(self.loop_nr),
            "record"
        )
        
    def mute(self):
        self.client.send_osc_message(
            "/sl/{}/hit".format(self.loop_nr),
            "mute"
        )
        
class Property:
    def __init__(self,initial_value,max_delta = 200):
        self.last_update = -1
        self.trend = 0
        self.value = initial_value
        self.max_delta = max_delta
    def get_value(self):
        now = get_millis()
        time_delta = self._get_update_time_delta(now)
        return self.value + (time_delta * self.trend)
    
    def set_value(self,new_value):
        now = get_millis()
        time_delta = self._get_update_time_delta(now)
        if time_delta > 0:
            value_delta = new_value - self.value
            new_trend = value_delta / time_delta
            self.trend = new_trend
        else:
            self.trend = 0
        
        self.last_update = now
        self.value = new_value
    
    def _get_update_time_delta(self,now):
        delta = now - self.last_update
        if self.last_update == -1 or delta > self.max_delta:
            return 0
        else:
            return delta

def get_millis():
    return time.time() * 1000

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
