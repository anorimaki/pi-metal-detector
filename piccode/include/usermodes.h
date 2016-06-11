#ifndef USER_MODES_H
#define	USER_MODES_H

typedef void (*ModeFuction) ();

void mode_init() ;
int1 mode_changed();
void mode_execute_current();


void mode_main();


#endif	/* INPUT_MODE_H */

