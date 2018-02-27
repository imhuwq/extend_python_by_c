#include <Python.h>

PyObject *sum(PyObject *self, PyObject *args) {
  int num1, num2;
  PyArg_ParseTuple(args, "ii", &num1, &num2);
  return Py_BuildValue("i", num1 + num2);
}

static PyMethodDef MymathMethods[] = {
    {"sum", sum, METH_VARARGS, "add up two numbers and return their sum"},
    {NULL}
};

static struct PyModuleDef mymath_module = {
    PyModuleDef_HEAD_INIT,
    "mymath",
    PyDoc_STR("A hello world example to demonstrate writing a python module in c language"),
    -1,
    MymathMethods
};

PyMODINIT_FUNC PyInit_mymath(void) {
  PyObject *m = PyModule_Create(&mymath_module);
  return m;
}
