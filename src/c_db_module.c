#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <c_db/c_db.h>
#include <c_core/c_core.h>

static PyObject *py_cd_database_open(PyObject *self, PyObject *args);
static PyObject *py_cd_database_close(PyObject *self, PyObject *args);

static PyMethodDef py_c_db_methods[] =
{
	{"db_open", py_cd_database_open, METH_VARARGS, "Python interface for cd_database_open C library function"},
	{"db_close", py_cd_database_close, METH_VARARGS, "Python interface for cd_database_close C library function"},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef py_c_db_module =
{
	PyModuleDef_HEAD_INIT,
	"py_c_db",
	"Python interface for the c_db C library ",
	-1,
	py_c_db_methods
};

PyMODINIT_FUNC PyInit_py_c_db(void) {
	return PyModule_Create(&py_c_db_module);
}

static PyObject *py_cd_database_open(PyObject *self, PyObject *args)
{
	const char *_db_name;

	/* Parse arguments */
	if(!PyArg_ParseTuple(args, "s", &_db_name))
	{
		return NULL;
	}

	CC_String db_name =
	{
		.data = _db_name,
		.length = strlen(_db_name)
	};

	CD_Database *db = cd_database_open(db_name);
	if(db == NULL)
	{
		CD_Error error = cd_get_last_error();
		PyErr_SetString(PyExc_IOError, error.message.data);
		return NULL;
	}

	return PyCapsule_New(db, NULL, NULL);
}

static PyObject *py_cd_database_close(PyObject *self, PyObject *args)
{
	PyObject *py_db;

	/* Parse arguments */
	if(!PyArg_ParseTuple(args, "O", &py_db))
	{
		return NULL;
	}

	CD_Database *db = (CD_Database *)PyCapsule_GetPointer(py_db, NULL);
	cd_database_close(db);

	Py_INCREF(Py_None);
	return Py_None;
}