#ifndef DUBLIN_WHISKEY_H
#define DUBLIN_WHISKEY_H

#include "Python.h"

typedef struct {
    PyObject_HEAD;
    PyObject *uuid;
    PyObject *name;
} WhiskeyObject;

extern PyTypeObject Whiskey_Type;

#define WhiskeyObject_Check(v)       (Py_TYPE(v) == &Whiskey_Type)

#endif