#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <c_db/c_db.h>
#include <c_core/c_core.h>

static PyObject *py_cd_database_open(PyObject *self, PyObject *args);
static PyObject *py_cd_database_close(PyObject *self, PyObject *args);
static PyObject *py_cd_table_create(PyObject *self, PyObject *args);

static PyMethodDef py_c_db_methods[] =
{
	{"db_open", py_cd_database_open, METH_VARARGS, "Python interface for cd_database_open C library function"},
	{"db_close", py_cd_database_close, METH_VARARGS, "Python interface for cd_database_close C library function"},
	{"table_create", py_cd_table_create, METH_VARARGS, "Python interface for cd_table_create C library function"},
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

static PyObject *py_cd_table_create(PyObject *self, PyObject *args)
{
	uint64_t _error = 0;

	PyObject *py_db;
	const char *_db_name;
	PyListObject *py_attributes;

	if(!PyArg_ParseTuple(args, "OsO!", &py_db, &_db_name, &PyList_Type, &py_attributes))
	{
		_error = 1;
		goto _return;
	}

	CD_Database *db = (CD_Database *)PyCapsule_GetPointer(py_db, NULL);

	CC_String table_name =
	{
		.data = _db_name,
		.length = strlen(_db_name)
	};

	uint64_t attribute_count = PyList_Size(py_attributes);

	CD_Attribute *attributes = malloc(sizeof(CD_Attribute) * attribute_count);
	
	for(uint64_t i = 0; i < attribute_count; i++)
	{
		PyObject *element = PyList_GetItem(py_attributes, i);

		if(!PyObject_TypeCheck(element, &PyDict_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute needs to be a dictionary.");
			_error = 1;
			goto attributes_free;
		}

		CD_Attribute *attribute = attributes + i;

		const char *key_type_string = "type";
		PyObject *key_type = PyUnicode_FromStringAndSize(key_type_string, strlen(key_type_string));
		PyObject *py_type = PyDict_GetItem(element, key_type);

		if(!PyObject_TypeCheck(py_type, &PyLong_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute type should be an integer.");
			_error = 1;
			goto attributes_free;
		}

		attribute->type = PyLong_AsLongLong(py_type);

		const char *key_count_string = "count";
		PyObject *key_count = PyUnicode_FromStringAndSize(key_count_string, strlen(key_count_string));
		PyObject *py_count = PyDict_GetItem(element, key_count);

		if(!PyObject_TypeCheck(py_count, &PyLong_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute count should be an integer.");
			_error = 1;
			goto attributes_free;
		}

		attribute->count = PyLong_AsLongLong(py_count);

		const char *key_name_string = "name";
		PyObject *key_name = PyUnicode_FromStringAndSize(key_name_string, strlen(key_name_string));
		PyObject *py_name = PyDict_GetItem(element, key_name);

		if(!PyObject_TypeCheck(py_name, &PyUnicode_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute name should be a string.");
			_error = 1;
			goto attributes_free;
		}

		memset(attribute->name, 0, 256);
		strcpy_s(attribute->name, 256, PyUnicode_AsUTF8(py_name));
	}

	uint64_t success = cd_table_create(db, table_name, attribute_count, attributes);
	if(!success)
	{
		CD_Error error = cd_get_last_error();
		PyErr_SetString(PyExc_IOError, error.message.data);
		_error = 1;
		goto attributes_free;
	}

attributes_free:
	free(attributes);

_return:
	if(_error)
	{
		return NULL;
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}
