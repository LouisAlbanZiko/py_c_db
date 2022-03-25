#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <c_core/c_core.h>
#include <c_db/c_db.h>

// database
extern PyObject *py_cd_database_exists(PyObject *self, PyObject *args);
extern PyObject *py_cd_database_create(PyObject *self, PyObject *args);

extern PyObject *py_cd_database_open(PyObject *self, PyObject *args);
extern PyObject *py_cd_database_close(PyObject *self, PyObject *args);

// table
extern PyObject *py_cd_table_exists(PyObject *self, PyObject *args);
extern PyObject *py_cd_table_create(PyObject *self, PyObject *args);

extern PyObject *py_cd_table_open(PyObject *self, PyObject *args);
extern PyObject *py_cd_table_close(PyObject *self, PyObject *args);

extern PyObject *py_cd_table_attribute_by_name(PyObject *self, PyObject *args);
extern PyObject *py_cd_table_attribute_by_index(PyObject *self, PyObject *args);
extern PyObject *py_cd_table_stride(PyObject *self, PyObject *args);
extern PyObject *py_cd_table_count(PyObject *self, PyObject *args);

extern PyObject *py_cd_table_insert(PyObject *self, PyObject *args);
extern PyObject *py_cd_table_select(PyObject *self, PyObject *args);
