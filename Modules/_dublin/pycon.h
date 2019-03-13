#ifndef DUBLIN_PYCON_H
#define DUBLIN_PYCON_H

#include "Python.h"

typedef struct {
    PyObject_HEAD;
    PyObject *name;
    PyObject *start_on;
    PyObject *stop_on;
} PyConObject;

extern PyTypeObject dublin_PyConType;

int PyCon_init(PyConObject *self, PyObject *args, PyObject *kwds);
// void PyCon_dealloc(PyConObject *self);

#endif