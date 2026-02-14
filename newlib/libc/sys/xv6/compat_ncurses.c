#include <sgtty.h>
#include <sys/ioctl.h>
#include <termios.h>

int gtty(int fd, struct sgttyb *buf) {
    struct termios term;

    if (!buf) {
        return -1;
    }
    if (tcgetattr(fd, &term) < 0) {
        return -1;
    }

    buf->sg_ispeed = (char)term.c_ispeed;
    buf->sg_ospeed = (char)term.c_ospeed;
    buf->sg_erase = (char)term.c_cc[VERASE];
    buf->sg_kill = (char)term.c_cc[VKILL];
    buf->sg_flags = 0;

    if ((term.c_lflag & ICANON) == 0) {
        if (term.c_lflag & ISIG) {
            buf->sg_flags |= CBREAK;
        } else {
            buf->sg_flags |= RAW;
        }
    }

    return 0;
}

int stty(int fd, const struct sgttyb *buf) {
    struct termios term;

    if (!buf) {
        return -1;
    }
    if (tcgetattr(fd, &term) < 0) {
        return -1;
    }

    if (buf->sg_flags & RAW) {
        cfmakeraw(&term);
    } else if (buf->sg_flags & CBREAK) {
        term.c_lflag &= ~ICANON;
        term.c_lflag |= ISIG;
    } else {
        term.c_lflag |= ICANON;
        term.c_lflag |= ISIG;
    }

    return tcsetattr(fd, TCSANOW, &term);
}

extern int wattr_on(void *win, int attrs, void *opts) __attribute__((weak));

int wattrset(void *win, int attrs) {
    if (wattr_on) {
        return wattr_on(win, attrs, 0);
    }
    return 0;
}
