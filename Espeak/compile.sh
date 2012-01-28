gcc `pkg-config gstreamer-0.10  gstreamer-plugins-base-0.10 --cflags` -o espeak_stream.so espeak_stream.c `pkg-config gstreamer-0.10 gstreamer-plugins-base-0.10 --libs`

if [ $? -eq 0 ]; 
then
	echo "ESPEAK COMPILED. RUNNING."
	./espeak_stream.so "it is nice to be important, but it is more important to be nice."
else
	echo "UNABLE TO COMPILE"
	exit 1;
fi
