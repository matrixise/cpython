// #include "Python.h"
#include "module.h"
#include "whiskey.h"
#include "pycon.h"

static PyObject *
dublin_new_whiskey(PyObject *module, PyObject *args)
{
    PyObject *argList = Py_BuildValue("(s)", "Teeling");
    PyObject *whiskey = PyObject_CallObject((PyObject *) &dublin_WhiskeyType, argList);
    Py_DECREF(argList);
    return whiskey;
}

PyDoc_STRVAR(dublin_new_whiskey__doc__,
"new_whiskey()\n\
\n\
Instanciate a new whiskey");

static PyObject *
dublin_pycons(PyObject *module, PyObject *Py_UNUSED(ignored))
{
    PyObject *obj = NULL;

    obj = PyList_New(0);
    if (obj == NULL) {
        return NULL;
    }

    PyObject *pycon = PyType_GenericNew(&dublin_PyConType, NULL, NULL);
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

PyDoc_STRVAR(dublin_pycons__doc__,
"pycons()\n\
\n\
Return a list of one dublin.PyCon");

static PyObject *
dublin_load_pycons(PyObject *module, PyObject *args)
{
    PyObject *sqlite = NULL;
    PyObject *connection = NULL;
    PyObject *cursor = NULL;
    PyObject *result = NULL;

    if (!PyUnicode_CheckExact(args)) {
        PyErr_SetString(PyExc_TypeError, "filename must be a string");
        Py_DECREF(args);
        return NULL;
    }
    sqlite = PyImport_ImportModuleNoBlock("sqlite3");
    if (sqlite == NULL) {
        goto Done;
    }

    connection = PyObject_CallMethod(sqlite, "connect", "O", args);
    if (connection == NULL) {
        goto Done;
    }

    cursor = PyObject_CallMethod(connection, "cursor", "()");
    if (cursor == NULL) {
        goto Done;
    }

    result = PyObject_CallMethod(cursor, "execute", "s", "SELECT id, name FROM whiskies");
    if (!result || PyErr_Occurred()) {
        Py_DECREF(cursor);
        Py_DECREF(connection);
        return NULL;
    }
    Py_DECREF(result);

    PyObject *collections = PyObject_CallMethod(cursor, "fetchall", "()");
    if (collections == NULL) {
        goto Done;
    }

    Py_ssize_t length = PyList_Size(collections);
    result = PyList_New(0);
    if (result == NULL) {
        Py_DECREF(collections);
        goto Done;
    }

    PyObject *item = NULL, *name = NULL;
    for (Py_ssize_t pos = 0; pos < length; pos++) {
        item = PyList_GetItem(collections, pos);
        name = PyTuple_GetItem(item, 1);

        Py_INCREF(name);
        PyConObject *pycon = (PyConObject *) PyType_GenericNew(&dublin_PyConType, NULL, NULL);
        pycon->name = name;
        PyList_Append(result, (PyObject *)pycon);

        Py_DECREF(pycon);
        name = NULL;
    }


    Py_DECREF(collections);

    Done:

    if (cursor) {
        PyObject *tmp = PyObject_CallMethod(cursor, "close", "()");
        Py_DECREF(tmp);
    }

    if (connection) {
        PyObject *tmp = PyObject_CallMethod(connection, "close", "()");
        Py_DECREF(tmp);
    }

    Py_XDECREF(cursor);
    Py_XDECREF(connection);
    Py_XDECREF(sqlite);
    return result;
}

PyDoc_STRVAR(dublin_load_pycons__doc__,
"load_pycons(filename)\n\
\n\
load the PyCons from a SQLite3 database");

static PyMethodDef dublin_methods[] = {
    {
        "new_whiskey",
        (PyCFunction) dublin_new_whiskey,
        METH_NOARGS,
        dublin_new_whiskey__doc__
    },
    {
        "pycons",
        (PyCFunction) dublin_pycons,
        METH_NOARGS,
        dublin_pycons__doc__
    },
    {
        "load_pycons",
        (PyCFunction) dublin_load_pycons,
        METH_O,
        dublin_load_pycons__doc__
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
};


PyMODINIT_FUNC
PyInit_dublin(void)
{
    PyObject *module = NULL;

    if (PyType_Ready(&dublin_WhiskeyType) < 0) {
        return NULL;
    }

    if (PyType_Ready(&dublin_PyConType) < 0) {
        return NULL;
    }

    module = PyModule_Create(&dublin_module);
    if (module == NULL) {
        return NULL;
    }
    Py_INCREF(&dublin_WhiskeyType);
    PyModule_AddObject(module, "Whiskey", (PyObject *) &dublin_WhiskeyType);

    Py_INCREF(&dublin_PyConType);
    PyModule_AddObject(module, "PyCon", (PyObject *) &dublin_PyConType);

    return module;
}