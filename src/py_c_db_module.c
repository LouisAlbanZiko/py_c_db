#include "py_c_db.h"

PyMethodDef py_c_db_methods[] =
{
	{"database_exists", py_cd_database_exists, METH_VARARGS, "Python interface function for cd_database_exists c_db library function."},
	{"database_create", py_cd_database_create, METH_VARARGS, "Python interface function for cd_database_create c_db library function."},
	{"database_open", py_cd_database_open, METH_VARARGS, "Python interface function for cd_database_open c_db library function."},
	{"database_close", py_cd_database_close, METH_VARARGS, "Python interface function for cd_database_close c_db library function."},

	{"table_exists", py_cd_table_exists, METH_VARARGS, "Python interface function for cd_table_exists c_db library function."},
	{"table_create", py_cd_table_create, METH_VARARGS, "Python interface function for cd_table_create c_db library function."},
	{"table_open", py_cd_table_open, METH_VARARGS, "Python interface function for cd_table_open c_db library function."},
	{"table_close", py_cd_table_close, METH_VARARGS, "Python interface function for cd_table_close c_db library function."},
	{"table_attribute_by_name", py_cd_table_attribute_by_name, METH_VARARGS, "Python interface function for cd_table_attribute_by_name c_db library function."},
	{"table_attribute_by_index", py_cd_table_attribute_by_index, METH_VARARGS, "Python interface function for cd_table_attribute_by_index c_db library function."},
	{"table_stride", py_cd_table_stride, METH_VARARGS, "Python interface function for cd_table_stride c_db library function."},
	{"table_count", py_cd_table_count, METH_VARARGS, "Python interface function for cd_table_count c_db library function."},
	{"table_insert", py_cd_table_insert, METH_VARARGS, "Python interface function for cd_table_insert c_db library function."},
	{"table_select", py_cd_table_select, METH_VARARGS, "Python interface function for cd_table_select c_db library function."},
	
	{NULL, NULL, 0, NULL}
};

struct PyModuleDef py_c_db_module =
{
	PyModuleDef_HEAD_INIT,
	"py_c_db",
	"Python interface for the c_db C library ",
	-1,
	py_c_db_methods
};

PyMODINIT_FUNC PyInit_py_c_db(void) {
	PyObject* module = PyModule_Create(&py_c_db_module);

	// add type constants
	PyModule_AddIntConstant(module, "TYPE_BYTE", CD_TYPE_BYTE);
	PyModule_AddIntConstant(module, "TYPE_UINT", CD_TYPE_UINT);
	PyModule_AddIntConstant(module, "TYPE_SINT", CD_TYPE_SINT);
	PyModule_AddIntConstant(module, "TYPE_FLOAT", CD_TYPE_FLOAT);
	PyModule_AddIntConstant(module, "TYPE_CHAR", CD_TYPE_CHAR);
	PyModule_AddIntConstant(module, "TYPE_WCHAR", CD_TYPE_WCHAR);
	PyModule_AddIntConstant(module, "TYPE_VARCHAR", CD_TYPE_VARCHAR);
	PyModule_AddIntConstant(module, "TYPE_WVARCHAR", CD_TYPE_WVARCHAR);

	// add constraits
	PyModule_AddIntConstant(module, "CONSTRAINT_NONE", CD_CONSTRAINT_NONE);
	PyModule_AddIntConstant(module, "CONSTRAINT_NOT_NULL", CD_CONSTRAINT_NOT_NULL);
	PyModule_AddIntConstant(module, "CONSTRAINT_UNIQUE", CD_CONSTRAINT_UNIQUE);

	return module;
}
