// (c) Tord Wessman 2012
// Feel free to do what you like with code.
// 
// This example demonstrate how to link several elements to a 
// espeak gst-plugin source to create a baby-like robot voice.
//
// This example was taken from another context (where i use the interop
// services in C# to create a C# wrapper) and is therefore not used
// here.

#include "espeak_stream.h"

static GMainLoop *loop;

static GstElement *bin, 	// the containing all the elements
		*pipeline, 	 		// the pipeline for the bin
		*espeak,			// the espeak sink
		*convert0,
		*convert1,
		*convert2,
		*effect0,			// using speed filter from gst-bad
		*effect1,			// using audiocheblimit filter from gst-good
		*alsa_sink;			// sound card sink
	
static GstBus *bus;	//the bus element te transport messages from/to the pipeline

static int is_playing; //indicates that the player is busy
static int pitch;
static int rate;

static gboolean bus_call(GstBus *bus, GstMessage *msg, void *user_data)
{

	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_EOS: {
		//g_message("End-of-stream");
		g_main_loop_quit(loop);
		break;
	}
	case GST_MESSAGE_ERROR: {
		GError *err;
		gst_message_parse_error(msg, &err, NULL);
		//report error
		printf ("ERROR: %s", err->message);
		g_error_free(err);
		
		g_main_loop_quit(loop);
		
		break;
	} 
	case GST_MESSAGE_ELEMENT: {
		const GstStructure *str;
		str = gst_message_get_structure (msg);
		
		if (gst_structure_has_name(str,"espeak-mark") || 
			gst_structure_has_name(str,"espeak-word")) {
			//espeak messages
		}
		break;
	}
	//onther element specific message

	case GST_MESSAGE_APPLICATION: {

		const GstStructure *str;
		str = gst_message_get_structure (msg);
		 if (gst_structure_has_name(str,"turn_off"))
			{
				g_main_loop_quit(loop);
			}

		break;
	}
	default:
	
		break;
	}

		//printf("info: %i %s\n", (int)(msg->timestamp), GST_MESSAGE_TYPE_NAME (msg));

	return true;
}

//this method initiates the gobjects, the pipeline, the bus and the entire bin
int init() {
	gst_init (NULL, NULL);
	pitch = 100;
	rate = -100;

	// create the main loop
	loop = g_main_loop_new(NULL, FALSE);

	pipeline = gst_pipeline_new ("espeak_pipeline");
	bin = gst_bin_new ("espeak_bin");
	
	//initializing elements
	espeak = gst_element_factory_make ("espeak", "espeak_src");
	alsa_sink = gst_element_factory_make ("alsasink", "alsasink");
	convert0 = gst_element_factory_make ("audioconvert", "converter0");
	convert1 = gst_element_factory_make ("audioconvert", "converter1");
	convert2 = gst_element_factory_make ("audioconvert", "converter2");

	//creates and set the values for the speed filter	
	effect0 = gst_element_factory_make ("speed", "testish");
	g_object_set (G_OBJECT (effect0), "speed", 1.5, NULL);
	
	//creates and set the values for the cut off filter
	effect1 = gst_element_factory_make ("audiocheblimit", "cheb-LPF");
	g_object_set (G_OBJECT (effect1), "mode", 0, NULL);
	g_object_set (G_OBJECT (effect1), "cutoff", 3500.0, NULL);

	if (espeak == NULL || alsa_sink == NULL || pipeline == NULL || bin == NULL) {
		printf ("Unable to create elements.");
		return 0;
	}
	if (effect0 == NULL || effect1 == NULL) {
		printf ("Unable to create effects");
		return 0;
	}
	
	//initializes the espeak src with values
	g_object_set (G_OBJECT (espeak), "text", "", NULL);
	g_object_set (G_OBJECT (espeak), "pitch", pitch, NULL);
	g_object_set (G_OBJECT (espeak), "rate", rate, NULL);
	g_object_set (G_OBJECT (espeak), "track", 2, NULL);

	// create the bus for the pipeline:
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	//add the bus handler method:
	gst_bus_add_watch(bus, bus_call, NULL);

	gst_object_unref(bus);

	// Add the elements to the bin
	gst_bin_add_many (GST_BIN (bin), 
		espeak, 
		convert0,
		effect0,
		convert1,
		effect1,
		convert2,
		alsa_sink,
	NULL);

	// add the bin to the pipeline 
	gst_bin_add (GST_BIN (pipeline), bin);

	// link the elements and check for success
	if (!gst_element_link_many (
		espeak,
		convert0,
		effect0,
		convert1, 
		effect1,
		convert2,
		alsa_sink,
	NULL)) {
		printf ("Unable to link elements");
		return 0;
	}

	return 1;

}

//stop the main loop and dealocate resources
void stop() {
	g_main_loop_quit(loop);
	gst_object_unref(GST_OBJECT(pipeline));
	g_main_loop_unref (loop);
}

void play(const char* text) {

	is_playing = 1;
	g_object_set (G_OBJECT (espeak), "text", text, NULL);
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	g_main_loop_run(loop);
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	is_playing = 0;
}

void _ext_stop() {
	stop();
}
int _ext_init () {
	return init();
}

void _ext_set_pitch(int _pitch) {
	pitch = _pitch;
}

void _ext_set_rate(int _rate) {
	rate = _rate;
}

void _ext_speak(const char* text) {
	if (is_playing == 0)
		play(text);
}

void _ext_pause() {
	g_main_loop_quit(loop);
}

int _ext_is_playing () {
	return is_playing;
}

int _ext_get_pitch() {
	return pitch;
}
int _ext_get_rate() {
	return rate;
}

int main (int argc, char *argv[]) {

	is_playing = 0;
	if (init()) {
		if (argc > 1) {
			play (argv[1]);
			stop();
		}
		else 
			printf ("Nothing to say...");
	}
	else {
		printf ("unable to initialize");
		return -1;
	}
	
	return 0;
}
