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
    WhiskyObject *self = (WhiskyObject *) PyType_GenericNew(type, args, kwargs);
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
Whisky_init(WhiskyObject *self, PyObject *args, PyObject *kwargs)
{
    static char *keyword_list[] = {
        "name", NULL
    };

    PyObject *whisky_name = NULL;

    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keyword_list, &whisky_name)) {
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
Whisky_dealloc(WhiskyObject *self)
{
    Py_DECREF(self->uuid);
    Py_XDECREF(self->name);
    PyObject_Del(self);
}

static PyObject *
Whisky_str(PyObject *self)
{
    WhiskyObject *obj = (WhiskyObject *)self;
    return PyUnicode_FromFormat("<Whisky uuid='%S' - name='%S'>",
                                obj->uuid,
                                obj->name);
}

static PyMemberDef Whisky_members[] = {
    {"uuid", T_OBJECT, offsetof(WhiskyObject, uuid), READONLY, "Readonly Identifier"},
    {"name", T_OBJECT, offsetof(WhiskyObject, name), READWRITE, "Whisky name"},
    {NULL} // Sentinel
};

static PyObject *
Whisky_from_tuple(PyTypeObject *type, PyObject *args, PyObject *Py_UNUSED(ignored)) 
{
    PyObject *name = PyTuple_GET_ITEM(args, 0);
    PyObject *whisky = PyObject_CallObject((PyObject *) &Whisky_Type, name);
    return whisky;
}

PyDoc_STRVAR(
    Whisky_from_tuple__doc__,
    "Convert a tuple to a Whisky"
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
    .tp_str = Whisky_str,
};

static PyObject *
dublin_new_whisky(PyObject *module, PyObject *args)
{
    PyObject *argList = Py_BuildValue("(s)", "Teeling");
    PyObject *whisky = PyObject_CallObject((PyObject *) &Whisky_Type, argList);
    Py_DECREF(argList);
    return whisky;
}

static PyMethodDef dublin_methods[] = {
    {
        "new_whisky",
        (PyCFunction) dublin_new_whisky,
        METH_NOARGS,
        PyDoc_STR("New Whisky")
    },
    {NULL},
};

PyDoc_STRVAR(
    dublin_module__doc__,
    "Dublin Module"
);

static PyModuleDef dublin_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "dublin",
    .m_doc = dublin_module__doc__,
    .m_size = 0,
    .m_methods = dublin_methods
    // NULL,
    // NULL,
    // NULL,
    // NULL,
    // NULL
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
    Py_INCREF(&Whisky_Type);
    PyModule_AddObject(module, "Whisky", (PyObject *) &Whisky_Type);
    return module;
}