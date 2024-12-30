#ifndef LOGGER_H
#define LOGGER_H

#define CHECK_ALLOC(ptr) \
    if (!(ptr)) {        \
        return -1        \
    }

#endif