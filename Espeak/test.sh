gst-launch espeak text="hello mister mister mister!?" !audioconvert ! speed speed=1.5 ! audioconvert ! alsasink
# ladspa-delay-5s Delay=0.2
#ladspa-grain-scatter 
#audioconvert ! audio/x-raw-float,channels=1,width=32,rate=44100,endianness=1234 !
