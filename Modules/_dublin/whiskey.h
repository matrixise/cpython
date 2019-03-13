#ifndef DUBLIN_WHISKEY_H
#define DUBLIN_WHISKEY_H

#include "Python.h"

typedef struct {
    PyObject_HEAD;
    PyObject *uuid;
    PyObject *name;
} WhiskeyObject;

extern PyTypeObject dublin_WhiskeyType;

#define WhiskeyObject_Check(v)       (Py_TYPE(v) == &dublin_WhiskeyType)

#endif