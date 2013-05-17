#include "const.h"
#include "types.h"
#include "hal.h"
#include "kernel.h"
#include "tty.h"
#include "irq.h"
#include "debug.h"

pid_t TTY;

void init_tty(void);
void ttyd(void);
void send_keymsg(void);

void init_console();

void init_tty(void) {
	add_irq_handle(1, send_keymsg);
	init_console();
}

