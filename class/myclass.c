#include <Python.h>
#include <structmember.h>

typedef struct {
  PyObject_HEAD
  PyObject *name;
  PyObject *value;
} record_object;

static PyMemberDef record_members[] = {
    {NULL}
};

static PyObject *record_new(PyTypeObject *type, PyObject *bases, PyObject *attributes) {
  record_object *self;
  self = (record_object *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }

    self->value = Py_None;

  }
  return (PyObject *) self;
}

static int record_init(record_object *self, PyObject *args, PyObject *kwargs) {
  PyObject *name = NULL, *value = NULL, *tmp;

  static char *kwlist[] = {"name", "value", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", kwlist,
                                   &name, &value))
    return -1;

  if (name) {
    if (!PyUnicode_Check(name)) {
      PyErr_SetString(PyExc_TypeError, "The name attribute must be a string");
      return -1;
    }
    tmp = self->name;
    Py_INCREF(name);
    self->name = name;
    Py_XDECREF(tmp);
  }

  if (value) {
    tmp = self->value;
    Py_INCREF(value);
    self->value = value;
    Py_XDECREF(tmp);
  }

  return 0;
}

static PyObject *record_print(record_object *self) {
  if (self->name == NULL) {
    PyErr_SetString(PyExc_AttributeError, "first");
    return NULL;
  }

  if (self->value == NULL) {
    PyErr_SetString(PyExc_AttributeError, "last");
    return NULL;
  }

  return PyUnicode_FromFormat("%S %S", self->name, self->value);
}

static PyObject *record_object_get_name(record_object *self, void *closure) {
  Py_INCREF(self->name);
  return self->name;
}

static int record_object_set_name(record_object *self, PyObject *value, void *closure) {
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete name");
    return -1;
  }

  if (!PyUnicode_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "The name attribute value must be a string");
    return -1;
  }

  Py_DECREF(self->name);
  Py_INCREF(value);
  self->name = value;

  return 0;
}

static PyObject *record_object_get_value(record_object *self, void *closure) {
  Py_INCREF(self->value);
  return self->value;
}

static int record_object_set_value(record_object *self, PyObject *value, void *closure) {
  PyObject *tmp;
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete value");
    return -1;
  }

  tmp = self->value;
  Py_INCREF(value);
  self->value = value;
  Py_DECREF(tmp);

  return 0;
}

static PyGetSetDef record_getseters[] = {
    {"name", (getter) record_object_get_name, (setter) record_object_set_name, "name property", NULL},
    {"value", (getter) record_object_get_value, (setter) record_object_set_value, "value property", NULL},
    {NULL}
};

static PyMethodDef record_methods[] = {
    {"print", (PyCFunction) record_print, METH_NOARGS, "Print name and value"},
    {NULL}
};

static PyTypeObject record_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "myclass.Record",
    sizeof(record_object),
    0
};

static PyModuleDef myclass_module = {
    PyModuleDef_HEAD_INIT,
    "myclass",
    "a example module for defining built-in data type",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_myclass(void) {
  PyObject *module;
  module = PyModule_Create(&myclass_module);
  if (module == NULL)
    return NULL;

  record_type.tp_doc = "built-in Record type";
  record_type.tp_flags = Py_TPFLAGS_DEFAULT;
  record_type.tp_new = record_new;
  record_type.tp_init = (initproc) record_init;
  record_type.tp_members = record_members;
  record_type.tp_methods = record_methods;
  record_type.tp_getset = record_getseters;
  if (PyType_Ready(&record_type) < 0) {
    return NULL;
  }

  PyModule_AddObject(module, "Record", (PyObject *) &record_type);
  return module;
}
