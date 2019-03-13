// #include "Python.h"
#include "module.h"
#include "whiskey.h"
#include "pycon.h"

static PyObject *
dublin_new_whiskey(PyObject *module, PyObject *args)
{
    PyObject *argList = Py_BuildValue("(s)", "Teeling");
    PyObject *whiskey = PyObject_CallObject((PyObject *) &Whiskey_Type, argList);
    Py_DECREF(argList);
    return whiskey;
}

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
    PyObject *sqlite = NULL;
    PyObject *connection = NULL;
    PyObject *cursor = NULL;
    PyObject *result = NULL;

    if (!PyUnicode_CheckExact(args)) {
        PyErr_SetString(PyExc_TypeError, "filename must be a string");
        Py_DECREF(args);
        // Py_DECREF(args);
        return NULL;
    }
    sqlite = PyImport_ImportModuleNoBlock("sqlite3");
    if (sqlite == NULL) {
        goto Done;
    }

    // // _Py_IDENTIFIER();
    connection = PyObject_CallMethod(sqlite, "connect", "O", args);
    if (connection == NULL) {
        goto Done;
    }

    // PyObject_Print(connection, stderr, 0);

    cursor = PyObject_CallMethod(connection, "cursor", "()");
    if (cursor == NULL) {
        goto Done;
    }

    // fprintf(stderr, "\ncursor(before): %p %ld\n", cursor, Py_REFCNT(cursor));
    result = PyObject_CallMethod(cursor, "execute", "s", "SELECT id, name FROM whiskies");
    if (!result || PyErr_Occurred()) {
        Py_DECREF(cursor);
        Py_DECREF(connection);
        return NULL;
        // Py_XDECREF(result);
        // goto Done;
    }
    // fprintf(stderr, "cursor(after): %p %ld\n", cursor, Py_REFCNT(cursor));
    // fprintf(stderr, "result: %p %ld\n", result, Py_REFCNT(result));
    // PyObject_Print(result, stderr, 0);
    Py_DECREF(result);

    // fprintf(stderr, "cursor(before): %p %ld\n", cursor, Py_REFCNT(cursor));
    PyObject *collections = PyObject_CallMethod(cursor, "fetchall", "()");
    // fprintf(stderr, "cursor(after): %p %ld %s\n", cursor, Py_REFCNT(cursor), Py_TYPE(cursor)->tp_name);
    // fprintf(stderr, "collections: %p %ld %s\n", collections, Py_REFCNT(collections), Py_TYPE(collections)->tp_name);
    if (collections == NULL) {
        goto Done;
    }

    Py_ssize_t length = PyList_Size(collections);
    // fprintf(stderr, "Length: %ld\n", length);
    // PyObject_Print(collections, stderr, 0);
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
        PyConObject *pycon = (PyConObject *) PyType_GenericNew(&PyCon_Type, NULL, NULL);
        pycon->name = name;
        PyList_Append(result, (PyObject *)pycon);

        Py_DECREF(pycon);
        name = NULL;
    }


    Py_DECREF(collections);

    Done:

    // Py_DECREF(result);
    if (cursor) {
        PyObject *tmp = PyObject_CallMethod(cursor, "close", "()");
        Py_DECREF(tmp);
    }

    if (connection) {
        PyObject *tmp = PyObject_CallMethod(connection, "close", "()");
        Py_DECREF(tmp);
    }
    // PyObject_Print(result, stderr, 0);

    Py_XDECREF(cursor);
    Py_XDECREF(connection);
    Py_XDECREF(sqlite);
    // Py_RETURN_NONE;
    return result;
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