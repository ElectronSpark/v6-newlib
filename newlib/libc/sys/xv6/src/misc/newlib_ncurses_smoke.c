#include <curses.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    setenv("TERMINFO", "/usr/share/terminfo", 1);
    if (getenv("TERM") == 0) {
        setenv("TERM", "xterm", 1);
    }
    initscr();
    printw("ncurses smoke\n");
    refresh();
    endwin();
    printf("ncurses smoke: OK\n");
    return 0;
}
