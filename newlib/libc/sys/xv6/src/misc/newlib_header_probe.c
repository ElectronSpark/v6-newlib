#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#ifndef SIGWINCH
#error "SIGWINCH is required for ncurses resize handling"
#endif

int main(void) {
    struct termios term;
    struct winsize ws;
    (void)term;
    (void)ws;
    return SIGWINCH > 0 ? 0 : 1;
}
