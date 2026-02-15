/*
 * rawecho_test.c - Minimal test for non-canonical TTY echo
 *
 * Sets terminal to raw mode (ICANON=0, ECHO=0), then loops:
 *   read(0, &c, 1) → write(1, &c, 1)
 *
 * If characters echo immediately: kernel TTY non-canonical mode works.
 * If characters only appear after Enter: tcsetattr or tty_read is broken.
 *
 * Press 'q' to exit.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/termios.h>

int main(void) {
    struct termios old_t, new_t;

    /* Save original terminal settings */
    if (tcgetattr(0, &old_t) < 0) {
        write(1, "tcgetattr failed\n", 17);
        return 1;
    }

    /* Print diagnostics */
    char msg[128];
    int len;
    len = snprintf(msg, sizeof(msg),
                   "Before: c_lflag=0x%x ICANON=%d ECHO=%d\n",
                   (unsigned)old_t.c_lflag,
                   !!(old_t.c_lflag & ICANON),
                   !!(old_t.c_lflag & ECHO));
    write(1, msg, len);

    /* Set raw mode */
    new_t = old_t;
    new_t.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ISIG | IEXTEN);
    new_t.c_iflag &= ~(ICRNL | INLCR | IGNCR | IXON | IXOFF | ISTRIP);
    new_t.c_cc[VMIN] = 1;
    new_t.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &new_t) < 0) {
        write(1, "tcsetattr failed\n", 17);
        return 1;
    }

    /* Verify the change took effect */
    struct termios check;
    tcgetattr(0, &check);
    len = snprintf(msg, sizeof(msg),
                   "After:  c_lflag=0x%x ICANON=%d ECHO=%d\n",
                   (unsigned)check.c_lflag,
                   !!(check.c_lflag & ICANON),
                   !!(check.c_lflag & ECHO));
    write(1, msg, len);

    write(1, "Raw mode active. Type chars (q to quit):\n", 41);

    /* Echo loop */
    unsigned char c;
    for (;;) {
        ssize_t n = read(0, &c, 1);
        if (n <= 0)
            break;
        if (c == 'q') {
            write(1, "\nExiting.\n", 10);
            break;
        }
        /* Echo the character back immediately */
        write(1, &c, 1);
    }

    /* Restore original settings */
    tcsetattr(0, TCSANOW, &old_t);
    write(1, "\nRestored terminal.\n", 20);

    return 0;
}
