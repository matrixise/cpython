#include "whiskey.h"
#include "structmember.h"


static PyObject *
Whiskey_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    WhiskeyObject *self = (WhiskeyObject *) PyType_GenericNew(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }

    PyObject *uuid = PyUnicode_FromFormat("UUID: %p", self);
    if (uuid == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->uuid = uuid;
    return (PyObject *) self;
}

static int
Whiskey_init(WhiskeyObject *self, PyObject *args, PyObject *kwds)
{
    static char *keyword_list[] = {
        "name", NULL
    };

    PyObject *whisky_name = NULL;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "|O", keyword_list, &whisky_name)) {
        return -1;
    }

    if (whisky_name == NULL) {
        return 0;
    }
    Py_INCREF(whisky_name);
    if (!PyUnicode_Check(whisky_name)) {
        Py_DECREF(whisky_name);
        PyErr_SetString(PyExc_TypeError, "name must be a string");
        return -1;
    }
    self->name = whisky_name;
    return 0;
}

static void
Whiskey_dealloc(WhiskeyObject *self)
{
    Py_DECREF(self->uuid);
    Py_XDECREF(self->name);
    PyObject_Del(self);
}

static PyObject *
Whiskey_str(PyObject *self)
{
    WhiskeyObject *obj = (WhiskeyObject *)self;
    return PyUnicode_FromFormat("<Whiskey uuid='%S' - name='%S'>",
                                obj->uuid,
                                obj->name);
}

static PyMemberDef Whiskey_members[] = {
    {"uuid", T_OBJECT, offsetof(WhiskeyObject, uuid), READONLY, "Generated Identifier - READONLY"},
    {"name", T_OBJECT, offsetof(WhiskeyObject, name), READWRITE, "Whiskey name"},
    {NULL} // Sentinel
};

static PyObject *
Whiskey_from_tuple(PyTypeObject *type, PyObject *args, PyObject *Py_UNUSED(ignored)) 
{
    PyObject *name = PyTuple_GET_ITEM(args, 0);
    PyObject *whiskey = PyObject_CallObject((PyObject *) &dublin_WhiskeyType, name);
    return whiskey;
}

PyDoc_STRVAR(
    Whiskey_from_tuple__doc__,
    "Convert a tuple to a Whiskey\n"
    ">>> whiskey = Whiskey.from_tuple(('Teeling',)"
);

static PyMethodDef Whiskey_methods[] = {
    {
        "from_tuple",
        (PyCFunction)(void(*)(void)) Whiskey_from_tuple,
        METH_VARARGS | METH_CLASS,
        Whiskey_from_tuple__doc__
    },
    {NULL} // Sentinel
};

PyTypeObject dublin_WhiskeyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_doc = "Whiskey object",
    .tp_name = "dublin.Whiskey",
    .tp_basicsize = sizeof(WhiskeyObject),
    .tp_itemsize = 0,
    .tp_new = Whiskey_new,
    .tp_init = (initproc) Whiskey_init,
    .tp_dealloc = (destructor) Whiskey_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = Whiskey_members,
    .tp_methods = Whiskey_methods,
    .tp_str = Whiskey_str,
};
