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
SL_ATTRS = ("loop_len","loop_pos")
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
    
    # Propagate value
    sl_msg = "/sl/{0}/{1}"
    propagate_to_gui("/sl/{0}/{1}".format(loop,attr), value)
    
    # Propagate Relative Position
    if attr == "loop_pos" or attr == "loop_len":
        old_pos = get_last_value("loop_pos",loop)
        length = get_last_value("loop_len",loop)
        if not old_pos is None and not length is None:
            if old_pos != 0 and length != 0:
                propagate_to_gui("/sl/{0}/loop_pos_rel".format(loop), old_pos / length)
        
    
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
    fromgui_dispatcher = dispatcher.Dispatcher()
    fromgui_dispatcher.map(FROMGUI_ROOT+"/sl/*", fromgui_sl_handler)
    run_as_thread(register_sl_updates)
    fromgui_dispatcher.map("/fromAPP/sl*", fromapp_sl_handler)
    fromgui_dispatcher.map("*", catchall_handler)

    fromgui_server = osc_server.ThreadingOSCUDPServer(
        ("224.0.0.1", 54321), fromgui_dispatcher)
    print("Serving on {}".format(fromgui_server.server_address))
    
    #t = threading.Thread(target=fromgui_server.serve_forever)
    #t.daemon = True  # thread dies when main thread (only non-daemon thread) exits.
    #t.start()
    fromgui_server.serve_forever()
    
    
    
    #### remove this
    
    ############
    # TO GUI   #
    ############
    
    togui_dispatcher = dispatcher.Dispatcher()
    
    ## Sooperlooper
    togui_dispatcher.map(TOGUI_ROOT+"/*", togui_sl_handler)
    register_sl_updates()
    

    togui_server = osc_server.ThreadingOSCUDPServer(
        ("127.0.0.1", 54322), togui_dispatcher)
    print("Serving on {}".format(togui_server.server_address))
    togui_server.serve_forever()
