gcc `pkg-config gstreamer-0.10 --cflags` -o espeak_stream.so espeak_stream.c -shared -fPIC `pkg-config gstreamer-0.10 --libs`

if [ $? -eq 0 ]; 
then
	echo "ESPEAK COMPILED"
	./espeak_stream.so
else
	echo "UNABLE TO COMPILE"
	exit 1;
fi
