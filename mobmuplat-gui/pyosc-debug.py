import threading, time

from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import osc_message_builder
from pythonosc import udp_client

FROMGUI_ROOT = "/fromGUI"
TOGUI_ROOT = "/toGUI"

# SOOPERLOOPER
client_sl = udp_client.UDPClient("127.0.0.1", 9951)
LOOPS = 3
SL_ATTRS = ("loop_len","loop_pos","state")
SL_STATES = {
    -1 : "Unknown",
    0 : "Off",
    1 : "Wait Start",
    2 : "Recording",
    3 : "Wait Stop",
    4 : "Playing",
    5 : "Overdubbing",
    6 : "Multiplying",
    7 : "Inserting",
    8 : "Replacing",
    9 : "Delay",
    10 : "Muted",
    11 : "Scratching",
    12 : "One Shot",
    13 : "Substitute",
    14 : "Paused"
}
last_val = {}

# GUI
client_gui = udp_client.UDPClient("224.0.0.1", 54322)

def remove_root(addr):
    return addr.replace(FROMGUI_ROOT+"/","")

# IN:  /FROMGUI_ROOT/sl/<loop>/<action> [1==down|0==up]
# OUT: /sl/<loop>/[hit|down|up] <action>
def fromgui_sl_handler(unused_addr, args):
    print("G-->ME: {0} {1}".format(unused_addr,args))
    loop_action = remove_root(unused_addr).split("/")
    
    # Check if message is well formatted
    if len(loop_action) != 3:
        print("--> DISCARD: invalid message")
        return
    
    # Get variables
    loop = loop_action[1]
    action = loop_action[2]
    
    # Set button state
    state = ("up" if args == 0 else "down")
    sl_osc_addr = "/sl/{0}/{1}".format(loop,state)
    
    print("ME-->SL: {0} {1}".format(sl_osc_addr, action))
    msg = osc_message_builder.OscMessageBuilder(address = sl_osc_addr)
    msg.add_arg(action)
    msg = msg.build()
    client_sl.send(msg)

def fromapp_sl_handler(unused_addr, *args):
    
    loop = args[0]
    attr = args[1]
    value = args[2]
    
    key = "{0}{1}".format(attr,loop)
    last_val[key] = value
    
    sl_msg = "/sl/{0}/{1}"
    
    if attr == "state":
        # Propagate label state
        record_state = 0
        multiply_state = 0
        overdub_state = 0
        
        if value == 2: # Recording
            record_state = 1
        elif value == 5: # Overdubbing
            overdub_state = 1
        elif value == 6: # Multiplying
            multiply_state = 1
            
        propagate_to_gui(sl_msg.format(loop,"record_label"), "highlight",record_state)
        propagate_to_gui(sl_msg.format(loop,"multiply_label"), "highlight", multiply_state)
        propagate_to_gui(sl_msg.format(loop,"overdub_label"), "highlight", overdub_state)
        
        # Propagate status --> id to string
        propagate_to_gui(sl_msg.format(loop,"status_text"), SL_STATES[value])
    
    elif attr == "loop_pos":
        # Propagate Relative Position
        old_pos = get_last_value("loop_pos",loop)
        length = get_last_value("loop_len",loop)
        if not old_pos is None:
            rel_pos = 0
            if length != 0:
                rel_pos = old_pos / length
            propagate_to_gui(sl_msg.format(loop,"loop_pos_rel"), rel_pos)
    else:
        # Propagate raw value
        
        propagate_to_gui(sl_msg.format(loop,attr), value)
    
    
        
    
def propagate_to_gui(msg, *attrs):
    msg = osc_message_builder.OscMessageBuilder(address = TOGUI_ROOT + msg)
    for attr in attrs:
        msg.add_arg(attr)
    msg = msg.build()
    client_gui.send(msg)
    

def get_last_value(attr,loop):
    key = "{0}{1}".format(attr,loop)
    return last_val[key] if key in last_val else None
    
    
def catchall_handler(unused_addr, *args):
    print("{0} {1}".format(unused_addr,args))

def register_sl_updates():
    while True:
    
        for loop in range(3):
            for attr in SL_ATTRS:
                # GET
                msg = osc_message_builder.OscMessageBuilder(address = "/sl/{0}/get".format(loop))
                msg.add_arg(attr)
                msg.add_arg("osc.udp://224.0.0.1:54321")
                msg.add_arg("/fromAPP/sl")
                msg = msg.build()
                client_sl.send(msg)
                
        # Set connection state
        msg = osc_message_builder.OscMessageBuilder(address = TOGUI_ROOT + "/heartbeat")
        msg = msg.build()
        client_gui.send(msg)
        
        time.sleep(0.1)
    
def togui_sl_handler(unused_addr, args):
    print(unused_addr, args)

def run_as_thread(f):
    t = threading.Thread(target=f)
    t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
    t.start()

if __name__ == "__main__":

    ############
    # FROM GUI #
    ############
    # Dispatcher
    fromgui_dispatcher = dispatcher.Dispatcher()
    fromgui_dispatcher.map(FROMGUI_ROOT+"/sl/*", fromgui_sl_handler)
    fromgui_dispatcher.map("/fromAPP/sl*", fromapp_sl_handler)
    fromgui_dispatcher.map("*", catchall_handler)
    
    # Update Loop
    run_as_thread(register_sl_updates)
    
    # Start Server
    fromgui_server = osc_server.ThreadingOSCUDPServer(
        ("224.0.0.1", 54321), fromgui_dispatcher)
    print("Serving on {}".format(fromgui_server.server_address))
    
    #t = threading.Thread(target=fromgui_server.serve_forever)
    #t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
    #t.start()
    fromgui_server.serve_forever()
    
