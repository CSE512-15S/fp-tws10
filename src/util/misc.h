#ifndef MISC_H
#define MISC_H

static inline int nextP2 (int a) {
    int rval=1;
    while(rval<a) rval<<=1;
    return rval;
}

#endif // MISC_H
