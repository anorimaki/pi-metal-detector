#ifndef USER_MODES_H
#define	USER_MODES_H

typedef void (*ModeFuction) ();

void mode_init() ;

#define NO_MODE_BUTTON  0xFF
int8 mode_check_buttons();

int1 mode_changed();

void mode_execute_current();


void mode_main();


#endif	/* INPUT_MODE_H */

