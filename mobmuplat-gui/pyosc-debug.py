import argparse
import math

from pythonosc import dispatcher
from pythonosc import osc_server
from pythonosc import osc_message_builder
from pythonosc import udp_client

ROOT = "/fromGUI"
client_sl = udp_client.UDPClient("127.0.0.1", 9951)
client_gui = udp_client.UDPClient("224.0.0.1", 54322)

def remove_root(addr):
    return addr.replace(ROOT+"/","")

# IN:  /ROOT/sl/<loop>/<action> [1==down|0==up]
# OUT: /sl/<loop>/[hit|down|up] <action>
def print_sl_handler(unused_addr, args):
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
    

def print_volume_handler(unused_addr, args, volume):
    print("[{0}] ~ {1}".format(args[0], volume))

def print_compute_handler(unused_addr, args, volume):
    try:
        print("[{0}] ~ {1}".format(args[0], args[1](volume)))
    except ValueError: pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip",
        default="224.0.0.1", help="The ip to listen on")
    parser.add_argument("--port",
        type=int, default=54321, help="The port to listen on")
    args = parser.parse_args()

    dispatcher = dispatcher.Dispatcher()
    dispatcher.map("/fromGUI/sl/*", print_sl_handler)
    dispatcher.map("/volume", print_volume_handler, "Volume")
    dispatcher.map("/logvolume", print_compute_handler, "Log volume", math.log)

    server = osc_server.ThreadingOSCUDPServer(
        (args.ip, args.port), dispatcher)
    print("Serving on {}".format(server.server_address))
    server.serve_forever()
