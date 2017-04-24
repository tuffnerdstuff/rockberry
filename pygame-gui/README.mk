# Testing OSC communication on the command-line

## OSC Server
Receives messages from sooperlooper

oscdump "osc.udp://:9952"

## OSC Client
Sends messages to sooperlooper. See OSC doc here: http://essej.net/sooperlooper/doc_osc.html

oscsend "osc.udp://localhost:9951" /ping ss "osc.udp://127.0.0.1:9952" "/return"
