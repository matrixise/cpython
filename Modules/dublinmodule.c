#include "Python.h"
#include "structmember.h"

typedef struct {
    PyObject_HEAD;
    PyObject *uuid;
    PyObject *name;
} WhiskeyObject;

static PyTypeObject Whiskey_Type;

#define WhiskeyObject_Check(v)       (Py_TYPE(v) == &Whiskey_Type)

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
    PyObject *whiskey = PyObject_CallObject((PyObject *) &Whiskey_Type, name);
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

static PyTypeObject Whiskey_Type = {
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

static PyObject *
dublin_new_whiskey(PyObject *module, PyObject *args)
{
    PyObject *argList = Py_BuildValue("(s)", "Teeling");
    PyObject *whiskey = PyObject_CallObject((PyObject *) &Whiskey_Type, argList);
    Py_DECREF(argList);
    return whiskey;
}


typedef struct {
    PyObject_HEAD;
    PyObject *name;
    PyObject *start_on;
    PyObject *stop_on;
} PyConObject;

static PyTypeObject PyCon_Type;

static int
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

static PyTypeObject PyCon_Type = {
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

static PyObject *
dublin_pycons(PyObject *module, PyObject *Py_UNUSED(ignored))
{
    PyObject *obj = NULL;

    obj = PyList_New(0);
    if (obj == NULL) {
        return NULL;
    }

    PyObject *pycon = PyType_GenericNew(&PyCon_Type, NULL, NULL);
    if (pycon == NULL) {
        Py_DECREF(obj);
        return NULL;
    }
    PyCon_init((PyConObject *) pycon, NULL, NULL);
    if (PyList_Append(obj, pycon) < 0) {
        Py_DECREF(obj);
        return NULL;
    }
    Py_DECREF(pycon);
    return obj;
}

static PyObject *
dublin_load_pycons(PyObject *module, PyObject *args)
{
    if (!PyUnicode_CheckExact(args)) {
        PyErr_SetString(PyExc_TypeError, "filename must be a string");
        Py_DECREF(args);
        return NULL;
    }
    // PyErr_SetString(PyExc_NotImplementedError, )
    Py_RETURN_NONE;
}

static PyMethodDef dublin_methods[] = {
    {
        "new_whiskey",
        (PyCFunction) dublin_new_whiskey,
        METH_NOARGS,
        PyDoc_STR("New Whiskey")
    },
    {
        "pycons",
        (PyCFunction) dublin_pycons,
        METH_NOARGS,
        PyDoc_STR("Get the PyCons in Ireland")
    },
    {
        "load_pycons",
        (PyCFunction) dublin_load_pycons,
        METH_O,
        PyDoc_STR("Get the PyCons from a file")
    },
    {NULL},
};

PyDoc_STRVAR(
    dublin_module__doc__,
    "Dublin Module\n"
    "Website of Dublin: http://www.dublincity.ie/\n"
    "You love the Irish Whiskey -> Irish Whiskey Museum: https://www.irishwhiskeymuseum.ie/\n"
    "and I am a fan of the Teeling Distillery ;-)"
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

    if (PyType_Ready(&Whiskey_Type) < 0) {
        return NULL;
    }

    if (PyType_Ready(&PyCon_Type) < 0) {
        return NULL;
    }

    module = PyModule_Create(&dublin_module);
    if (module == NULL) {
        return NULL;
    }
    Py_INCREF(&Whiskey_Type);
    PyModule_AddObject(module, "Whiskey", (PyObject *) &Whiskey_Type);

    Py_INCREF(&PyCon_Type);
    PyModule_AddObject(module, "PyCon", (PyObject *) &PyCon_Type);

    return module;
}