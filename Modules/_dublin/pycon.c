#include "pycon.h"
#include "structmember.h"

int
PyCon_init(PyConObject *self, PyObject *args, PyObject *kwds)
{
    return 0;
}

static void
PyCon_dealloc(PyConObject *self)
{
    Py_XDECREF(self->name);
    Py_XDECREF(self->start_on);
    Py_XDECREF(self->stop_on);
    PyObject_Del(self);
}

static PyMemberDef PyCon_members[] = {
    {"name", T_OBJECT, offsetof(PyConObject, name), READONLY, "Name"},
    {"start_on", T_OBJECT, offsetof(PyConObject, start_on), READONLY, "Start"},
    {"finish_on", T_OBJECT, offsetof(PyConObject, stop_on), READONLY, "Finish"},
    {NULL},
};

PyTypeObject PyCon_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_doc = "PyCon object",
    .tp_name = "dublin.PyCon",
    .tp_basicsize = sizeof(PyConObject),
    .tp_itemsize = 0,
    .tp_new = NULL,
    .tp_init = (initproc) PyCon_init,
    .tp_dealloc = (destructor) PyCon_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = PyCon_members
};
