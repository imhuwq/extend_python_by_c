#include <Python.h>
#include <structmember.h>

typedef struct {
  PyObject_HEAD
  PyObject *name;
  PyObject *value;
  PyObject *weakref_list;
} record_object;

static PyMemberDef record_members[] = {
    {NULL}
};

static int record_traverse(record_object *self, visitproc visit, void *arg) {
  Py_VISIT(self->name);
  Py_VISIT(self->value);
  return 0;
}

static int record_clear(record_object *self) {
  Py_CLEAR(self->name);
  Py_CLEAR(self->value);
  return 0;
}

static void record_dealloc(record_object *self) {
  long count;
  PyObject_GC_UnTrack(self);
  record_clear(self);

  if (self->weakref_list != NULL) {
    PyObject_ClearWeakRefs((PyObject *) self);
  }

  count = PyLong_AsLong(PyDict_GetItem(Py_TYPE(self)->tp_dict, PyUnicode_FromString("count")));
  if (count > 0) {
    PyDict_SetItem(Py_TYPE(self)->tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(count - 1));
    PyType_Modified(Py_TYPE(self));
  }

  Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *record_new(PyTypeObject *type, PyObject *bases, PyObject *attributes) {
  record_object *self;
  long count;

  self = (record_object *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->weakref_list = NULL;

    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }

    self->value = Py_None;

    count = PyLong_AsLong(PyDict_GetItem(type->tp_dict, PyUnicode_FromString("count")));
    PyDict_SetItem(type->tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(count + 1));
    PyType_Modified(type);
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

static PyObject *record_get_count(PyTypeObject *type, PyObject *args) {
  PyObject *count = PyDict_GetItem(type->tp_dict, PyUnicode_FromString("count"));
  Py_INCREF(count);
  return count;
}

static PyObject *record_get_purpose(PyObject *null, PyObject *args) {
  return Py_BuildValue("s", "Using a pair of name and value to record anything you want");
}

static PyObject *record_richcmp(record_object *record1, record_object *record2, int op) {
  switch (op) {
    case Py_EQ: {
      if (record1->name == record2->name && record1->value == record2->value) {
        return Py_True;
      }
      return Py_False;
    }
    default: {
      PyErr_SetString(PyExc_Exception, "Not supported operation");
      return NULL;
    }
  }
}

static PyObject *record_str(record_object *self) {
  return PyUnicode_FromFormat("name: %S, value: %S", self->name, self->value);
}

static PyObject *record_repr(record_object *self) {
  return PyUnicode_FromFormat("myclass.Record(name=%S, value=%S)", self->name, self->value);
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
    {"get_count", (PyCFunction) record_get_count, METH_VARARGS | METH_CLASS, "Get instance count of this class"},
    {"get_purpose", (PyCFunction) record_get_purpose, METH_VARARGS | METH_STATIC, "Get the purpose this class"},
    {NULL}
};

static PyTypeObject record_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "myclass.Record",
    sizeof(record_object),
    0
};

typedef struct {
  record_object base_record;
} string_record_object;

static int string_record_object_set_value(record_object *self, PyObject *value, void *closure) {
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete value");
    return -1;
  }

  if (!PyUnicode_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "The value attribute value must be a string");
    return -1;
  }

  Py_DECREF(self->value);
  self->value = value;
  Py_INCREF(value);

  return 0;
}

static PyGetSetDef string_record_getseters[] = {
    {"name", (getter) record_object_get_name, (setter) record_object_set_name, "name property", NULL},
    {"value", (getter) record_object_get_value, (setter) string_record_object_set_value, "value property", NULL},
    {NULL}
};

static PyTypeObject string_record_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "myclass.StringRecord",
    sizeof(string_record_object),
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
  record_type.tp_str = (reprfunc) record_str;
  record_type.tp_repr = (reprfunc) record_repr;
  record_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE;
  record_type.tp_dealloc = (destructor) record_dealloc;
  record_type.tp_traverse = (traverseproc) record_traverse;
  record_type.tp_clear = (inquiry) record_clear;
  record_type.tp_weaklistoffset = offsetof(record_object, weakref_list);
  record_type.tp_new = record_new;
  record_type.tp_init = (initproc) record_init;
  record_type.tp_members = record_members;
  record_type.tp_methods = record_methods;
  record_type.tp_getset = record_getseters;
  record_type.tp_richcompare = (richcmpfunc) record_richcmp;
  record_type.tp_dict = PyDict_New();
  PyDict_SetItem(record_type.tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(0));
  if (PyType_Ready(&record_type) < 0) {
    return NULL;
  }
  PyModule_AddObject(module, "Record", (PyObject *) &record_type);

  string_record_type.tp_base = &record_type;
  string_record_type.tp_getset = string_record_getseters;
  if (PyType_Ready(&string_record_type) < 0) {
    return NULL;
  }
  PyModule_AddObject(module, "StringRecord", (PyObject *) &string_record_type);

  return module;
}
