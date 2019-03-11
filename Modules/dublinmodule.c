#include "Python.h"
#include "structmember.h"

typedef struct {
    PyObject_HEAD;
    PyObject *uuid;
    PyObject *name;
} WhiskyObject;

static PyTypeObject Whisky_Type;

#define WhiskyObject_Check(v)       (Py_TYPE(v) == &Whisky_Type)

static PyObject *
Whisky_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    WhiskyObject *self = NULL;
#ifdef Py_DEBUG
    PyObject_Print(args, stderr, 0);
    PyObject_Print(kwargs, stderr, 0);
#endif

    self = (WhiskyObject *) type->tp_alloc(type, 0);
    if (self == NULL) {
        self->name = NULL;
        goto error;
    }
    PyObject *uuid = PyUnicode_FromFormat("UUID: Whisky: %p", self);
    if (uuid == NULL) {
        goto error;
    }
    self->uuid = uuid;
    return (PyObject *) self;

    error:
        Py_DECREF(self);
        return NULL;
}

static int
Whisky_init(WhiskyObject *self, PyObject *args, PyObject *kwargs)
{
#ifdef Py_DEBUG
    fprintf(stderr, "Whisky_init: %p\n", self);
    PyObject_Print(args, stderr, 0);
    PyObject_Print(kwargs, stderr, 0);
#endif
    static char *keyword_list[] = {
        "name", NULL
    };

    PyObject *whisky_name = NULL;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keyword_list, &whisky_name)) {
        return -1;
    }

#ifdef Py_DEBUG
    fprintf(stderr, "Whisky_name: %p\n", whisky_name);
#endif
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
Whisky_dealloc(WhiskyObject *self)
{
#ifdef Py_DEBUG
    fprintf(stderr, "Whisky_dealloc: %p\n", self);
    fprintf(stderr, "name: %p\n", self->name);
#endif
    Py_DECREF(self->uuid);
    Py_XDECREF(self->name);
    PyObject_Del(self);
}

static PyMemberDef Whisky_members[] = {
    {"uuid", T_OBJECT, offsetof(WhiskyObject, uuid), READONLY, "Readonly Identifier"},
    {"name", T_OBJECT, offsetof(WhiskyObject, name), READWRITE, "Whisky name"},
    {NULL} // Sentinel
};

static PyObject *
Whisky_from_tuple(PyTypeObject *type, PyObject *args, PyObject *Py_UNUSED(ignored)) 
{
    fprintf(stderr, "Whisky_from_tuple\n");
    PyObject_Print((PyObject*) type, stderr, 0);
    PyObject_Print(args, stderr, 0);

    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    return NULL;
}

PyDoc_STRVAR(
    Whisky_from_tuple__doc__,
    "Convert a namedtuple to a record"
);

static PyMethodDef Whisky_methods[] = {
    {
        "from_tuple",
        (PyCFunction)(void(*)(void)) Whisky_from_tuple,
        METH_VARARGS | METH_CLASS,
        Whisky_from_tuple__doc__
    },
    {NULL} // Sentinel
};

static PyTypeObject Whisky_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_doc = "Whisky object",
    .tp_name = "dublin.Whisky",
    .tp_basicsize = sizeof(WhiskyObject),
    .tp_itemsize = 0,
    .tp_new = Whisky_new,
    .tp_init = (initproc) Whisky_init,
    .tp_dealloc = (destructor) Whisky_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = Whisky_members,
    .tp_methods = Whisky_methods,
};

PyDoc_STRVAR(
    dublin_module__doc__,
    "Dublin Module"
);

static PyModuleDef dublin_module = {
    PyModuleDef_HEAD_INIT,
    "dublin",
    dublin_module__doc__,
    0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_dublin(void)
{
    PyObject *module = NULL;

    if (PyType_Ready(&Whisky_Type) < 0) {
        return NULL;
    }

    module = PyModule_Create(&dublin_module);
    if (module == NULL) {
        return NULL;
    }
#ifdef Py_DEBUG
    fprintf(stderr, "PyInit_dublin: %p\n", module);
#endif
    Py_INCREF(&Whisky_Type);
    PyModule_AddObject(module, "Whisky", (PyObject *) &Whisky_Type);
    return module;
}