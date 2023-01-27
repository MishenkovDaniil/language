#ifndef LABELS_H
#define LABELS_H

const int MAXSIZE = 32;
static const int MAXLEN = 20;

static const int UNDEFINED = -1;

struct Label
{
    int value[MAXSIZE] = {};
    char name[MAXSIZE][MAXLEN] = {};
    int status[MAXSIZE] = {};
};

#endif /* LABELS_H */
