#include "main.h"
#include "inbuttons.h"
#include "input.h"


///////////////////////////////////////////////////////////////
// Switches
///////////////////////////////////////////////////////////////
#define CHANGE_CONFIDENT_THRESHOLD 2

struct InButton in_buttons[BUTTONS_SIZE];

void buttons_init() 
{
	for (int8 i = 0; i < BUTTONS_SIZE; ++i) {
		in_buttons[i].state = FALSE;
		in_buttons[i].changed = FALSE;
		in_buttons[i].change_confident_count = 0;
	}
}

void buttons_update( int8 switches_state )
{
	int8 i;
	for (i = 0; i < BUTTONS_SIZE; ++i ) {
		struct InButton* sw = &in_buttons[i];

		int1 current_state = switches_state & 0x01;
		switches_state >>= 1;
	
		if (current_state == sw->state) {
			sw->change_confident_count = 0;
			if (sw->state_time < (0xFFFF - IN_TIMER_PERIOD_MS))
				sw->state_time += IN_TIMER_PERIOD_MS;
		}
		else {
			if (++sw->change_confident_count == CHANGE_CONFIDENT_THRESHOLD) {
				sw->changed = TRUE;
				sw->state = current_state;
				sw->state_time = 0;
			}
		}
	}
}

// Wait until button releases or it has been pressed for more than 
// max_push_time ms. Returns true if max_push_time has been 
// reached.

int1 buttons_wait_for_release_timeout(int8 sw, int16 max_push_time)
{
	while (in_buttons[sw].state) {
		if (in_buttons[sw].state_time > max_push_time)
			return TRUE;
	}
	return FALSE;
}

void buttons_wait_for_release(int8 sw)
{
	while (in_buttons[sw].state);
}

int1 buttons_is_pressed(int8 sw)
{
	int1 ret = in_buttons[sw].state && in_buttons[sw].changed;
	in_buttons[sw].changed = FALSE;
	return ret;
}


