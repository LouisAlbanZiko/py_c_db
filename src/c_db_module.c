#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *spam_system(PyObject *self, PyObject *args);

static PyMethodDef SpamMethods[] = 
{
	{"system", spam_system, METH_VARARGS, "Execute a shell command."}
};

static PyModuleDef spammodule = 
{
	PyModuleDef_HEAD_INIT,
	"spam", // name of module
	NULL, // module documentation
	-1, // size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
	SpamMethods
};

PyMODINIT_FUNC PyInit_spam()
{
	return PyModule_Create(&spammodule);
}

static PyObject *spam_system(PyObject *self, PyObject *args)
{
	const char *command;
	int sts;
	if(!PyArg_ParseTuple(args, "s", &command))
	{
		return NULL;
	}
	sts = system(command);
	return PyLong_FromLong(sts);
}
