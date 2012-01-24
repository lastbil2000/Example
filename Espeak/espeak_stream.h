// (c) tord wessman 2012
// feel free to do what you like with code.

#include <gst/gst.h>
#include <stdbool.h>
#include <stdio.h>

//initializes the player. returns 1 if successfull
int _ext_init();
//speaks the text (!)
void _ext_speak(const char* text);
//set the pause between words (-100 to 100)
void _ext_set_rate(int _rate);
//set the voice pitch (-100 to 100)
void _ext_set_pitch(int _pitch);
// returns the current pitch
int _ext_get_pitch();
// returns the current rate
int _ext_get_rate();
//stop the player and dealocate resources
void _ext_stop();
//just stops the player
void _ext_pause();
//returns 1 if the player is playing

int _ext_is_playing ();

