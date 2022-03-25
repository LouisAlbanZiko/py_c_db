#include "py_c_db.h"

PyObject *py_cd_database_exists(PyObject *self, PyObject *args)
{
	const char *_db_name;

	if(!PyArg_ParseTuple(args, "s", &_db_name))
	{
		return NULL;
	}

	uint64_t exists = cd_database_exists(_db_name);
	
	return PyBool_FromLong(exists);
}

PyObject *py_cd_database_create(PyObject *self, PyObject *args)
{
	const char *_db_name;

	if(!PyArg_ParseTuple(args, "s", &_db_name))
	{
		return NULL;
	}

	uint64_t success = cd_database_create(_db_name);
	if(!success)
	{
		CD_Error error = cd_get_last_error();
		PyErr_SetString(PyExc_IOError, error.message.data);
		return NULL;
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject *py_cd_database_open(PyObject *self, PyObject *args)
{
	const char *_db_name;

	if(!PyArg_ParseTuple(args, "s", &_db_name))
	{
		return NULL;
	}

	CD_Database *db = cd_database_open(_db_name);
	if(db == NULL)
	{
		CD_Error error = cd_get_last_error();
		PyErr_SetString(PyExc_IOError, error.message.data);
		return NULL;
	}

	return PyCapsule_New(db, NULL, NULL);
}

PyObject *py_cd_database_close(PyObject *self, PyObject *args)
{
	PyObject *py_db;

	if(!PyArg_ParseTuple(args, "O", &py_db))
	{
		return NULL;
	}

	CD_Database *db = (CD_Database *)PyCapsule_GetPointer(py_db, NULL);
	cd_database_close(db);

	Py_INCREF(Py_None);
	return Py_None;
}
