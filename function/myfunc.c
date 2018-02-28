#include <Python.h>

PyObject *myfunc(PyObject *self, PyObject *args, PyObject *kwargs) {
  PyObject *items, *operator, *reporter = NULL;
  PyObject *result;
  PyObject * report;

  static char *kwlist[] = {"items", "operator", "reporter", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", kwlist, &items, &operator, &reporter)) {
    return NULL;
  }

  if (!PyList_Check(items)) {
    PyErr_SetString(PyExc_TypeError, "items must be a list");
    return NULL;
  }

  if (!PyCallable_Check(operator)) {
    PyErr_SetString(PyExc_TypeError, "operator must be a callable");
    return NULL;
  }

  if (reporter != NULL) {
    if (!PyCallable_Check(reporter)) {
      PyErr_SetString(PyExc_TypeError, "reporter must be a callable");
      return NULL;
    }
  }

  result = PyObject_CallObject(operator, Py_BuildValue("(O)", items));
  if (result == NULL)
    return NULL;
  Py_INCREF(result);

  if (reporter != NULL) {
    report = PyDict_New();
    if (args != NULL) PyDict_SetItemString(report, "args", args);
    if (kwargs != NULL) PyDict_SetItemString(report, "kwargs", kwargs);
    if (result != NULL) PyDict_SetItemString(report, "result", result);
    PyObject_CallObject(reporter, Py_BuildValue("(O)", report));
  }

  return result;
}

static PyMethodDef MymathMethods[] = {
    {"myfunc", myfunc, METH_VARARGS | METH_KEYWORDS, "a complex function"},
    {NULL}
};

static struct PyModuleDef myfunc_module = {
    PyModuleDef_HEAD_INIT,
    "myfunc",
    PyDoc_STR("built-in myfunc module"),
    -1,
    MymathMethods
};

PyMODINIT_FUNC PyInit_myfunc(void) {
  PyObject *m = PyModule_Create(&myfunc_module);
  return m;
}
