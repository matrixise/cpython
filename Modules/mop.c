#include <Python.h>

static PyObject *
mop_hello(PyObject *self, PyObject *args) 
{
    PyObject *display_str;
    
    display_str = PyUnicode_FromFormat("hi Pablo!\nwith the support from Yury, Elvis, Victor and Stephane\n");

    if (!display_str) {
        return NULL;
    }

    PyObject_Print(display_str, stdout, Py_PRINT_RAW);
    Py_DECREF(display_str);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef mop_methods[] = {
    {"hello", mop_hello, METH_VARARGS,
     "Show the mop message."},
    {NULL, NULL, 0, NULL}   /* Sentinel */
};

PyDoc_STRVAR(mop_doc,
"Import mop for Pablo, with the support from Yury, Victor, Elvis and Stephane\n"
);

static struct PyModuleDef mopmodule = {
    PyModuleDef_HEAD_INIT,
    "mop",
    mop_doc,
    0,
    mop_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_mop(void)
{
    return PyModule_Create(&mopmodule);
}
