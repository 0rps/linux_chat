#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <sys/ioctl.h>

inline bool setNonblock(int _socketFd) {
    int flags;

        /* If they have O_NONBLOCK, use the Posix way to do it */
    #if defined(O_NONBLOCK)
        /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
        if (-1 == (flags = fcntl(_socketFd, F_GETFL, 0)))
            flags = 0;
        return fcntl(_socketFd, F_SETFL, flags | O_NONBLOCK) == 0;
    #else
        /* Otherwise, use the old way of doing it */
        flags = 1;
        return ioctl(_socketFd, FIOBIO, &flags) == 0;
    #endif
}

#endif // UTILS_H
