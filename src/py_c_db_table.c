#include "py_c_db.h"

PyObject *py_cd_table_exists(PyObject *self, PyObject *args)
{
	PyObject *py_db;
	const char *table_name;

	if (!PyArg_ParseTuple(args, "Os", &py_db, &table_name))
	{
		return NULL;
	}

	CD_Database *db = (CD_Database *)PyCapsule_GetPointer(py_db, NULL);
	uint64_t exists = cd_table_exists(db, table_name);

	return PyBool_FromLong((long)exists);
}

PyObject *py_cd_table_create(PyObject *self, PyObject *args)
{
	PyObject *py_db;
	const char *table_name;
	PyObject *py_attributes;

	if (!PyArg_ParseTuple(args, "OsO!", &py_db, &table_name, &PyList_Type, &py_attributes))
	{
		return NULL;
	}

	uint64_t _error = 0;

	uint64_t attribute_count = PyList_Size(py_attributes);

	CD_Attribute *attributes = malloc(sizeof(CD_Attribute) * attribute_count);

	for (uint64_t i = 0; i < attribute_count; i++)
	{
		PyObject *element = PyList_GetItem(py_attributes, i);

		if (!PyObject_TypeCheck(element, &PyDict_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute needs to be a dictionary.");
			_error = 1;
			goto attributes_free;
		}

		CD_Attribute *attribute = attributes + i;

		const char *key_type_string = "type";
		PyObject *key_type = PyUnicode_FromStringAndSize(key_type_string, strlen(key_type_string));
		PyObject *py_type = PyDict_GetItem(element, key_type);

		if (!PyObject_TypeCheck(py_type, &PyLong_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute type should be an integer.");
			_error = 1;
			goto attributes_free;
		}

		attribute->type = PyLong_AsLongLong(py_type);

		const char *key_count_string = "count";
		PyObject *key_count = PyUnicode_FromStringAndSize(key_count_string, strlen(key_count_string));
		PyObject *py_count = PyDict_GetItem(element, key_count);

		if (!PyObject_TypeCheck(py_count, &PyLong_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute count should be an integer.");
			_error = 1;
			goto attributes_free;
		}

		attribute->count = PyLong_AsLongLong(py_count);

		const char *key_constraints_string = "constraints";
		PyObject *key_constraints = PyUnicode_FromStringAndSize(key_constraints_string, strlen(key_constraints_string));
		PyObject *py_constraints = PyDict_GetItem(element, key_constraints);

		if (!PyObject_TypeCheck(py_constraints, &PyLong_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute constraints should be an integer.");
			_error = 1;
			goto attributes_free;
		}

		attribute->constraints = PyLong_AsLongLong(py_constraints);

		const char *key_name_string = "name";
		PyObject *key_name = PyUnicode_FromStringAndSize(key_name_string, strlen(key_name_string));
		PyObject *py_name = PyDict_GetItem(element, key_name);

		if (!PyObject_TypeCheck(py_name, &PyUnicode_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute name should be a string.");
			_error = 1;
			goto attributes_free;
		}

		attribute->name = PyUnicode_AsUTF8(py_name);
	}

	CD_Database *db = (CD_Database *)PyCapsule_GetPointer(py_db, NULL);
	uint64_t success = cd_table_create(db, table_name, attribute_count, attributes);
	if (!success)
	{
		CD_Error error = cd_get_last_error();
		PyErr_SetString(PyExc_IOError, error.message.data);
		_error = 1;
		goto attributes_free;
	}

attributes_free:
	free(attributes);

	if (_error)
	{
		return NULL;
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject *py_cd_table_open(PyObject *self, PyObject *args)
{
	PyObject *py_db;
	const char *table_name;

	if (!PyArg_ParseTuple(args, "Os", &py_db, &table_name))
	{
		return NULL;
	}

	CD_Database *db = (CD_Database *)PyCapsule_GetPointer(py_db, NULL);
	CD_Table *table = cd_table_open(db, table_name);
	if (table == NULL)
	{
		CD_Error error = cd_get_last_error();
		PyErr_SetString(PyExc_IOError, error.message.data);
		return NULL;
	}

	return PyCapsule_New(table, NULL, NULL);
}

PyObject *py_cd_table_close(PyObject *self, PyObject *args)
{
	PyObject *py_table;

	if (!PyArg_ParseTuple(args, "O", &py_table))
	{
		return NULL;
	}

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);
	cd_table_close(table);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_cd_table_attribute_by_name(PyObject *self, PyObject *args)
{
	PyObject *py_table;
	const char *attribute_name;

	if (PyArg_ParseTuple(args, "Os", &py_table, &attribute_name))
	{
		return NULL;
	}

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);
	const CD_AttributeEx *attribute = cd_table_attribute_by_name(table, attribute_name);

	PyObject *dict = PyDict_New();

	PyDict_SetItemString(dict, "name", PyUnicode_FromString(attribute->name));
	PyDict_SetItemString(dict, "type", PyLong_FromUnsignedLongLong(attribute->type));
	PyDict_SetItemString(dict, "count", PyLong_FromUnsignedLongLong(attribute->count));
	PyDict_SetItemString(dict, "constraints", PyLong_FromUnsignedLongLong(attribute->constraints));
	PyDict_SetItemString(dict, "offset", PyLong_FromUnsignedLongLong(attribute->offset));
	PyDict_SetItemString(dict, "size", PyLong_FromUnsignedLongLong(attribute->size));

	return dict;
}

PyObject *py_cd_table_attribute_by_index(PyObject *self, PyObject *args)
{
	PyObject *py_table;
	uint64_t index;

	if (PyArg_ParseTuple(args, "OK", &py_table, &index))
	{
		return NULL;
	}

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);
	const CD_AttributeEx *attribute = cd_table_attribute_by_index(table, index);

	PyObject *dict = PyDict_New();

	PyDict_SetItemString(dict, "name", PyUnicode_FromString(attribute->name));
	PyDict_SetItemString(dict, "type", PyLong_FromUnsignedLongLong(attribute->type));
	PyDict_SetItemString(dict, "count", PyLong_FromUnsignedLongLong(attribute->count));
	PyDict_SetItemString(dict, "constraints", PyLong_FromUnsignedLongLong(attribute->constraints));
	PyDict_SetItemString(dict, "offset", PyLong_FromUnsignedLongLong(attribute->offset));
	PyDict_SetItemString(dict, "size", PyLong_FromUnsignedLongLong(attribute->size));

	return dict;
}

PyObject *py_cd_table_stride(PyObject *self, PyObject *args)
{
	PyObject *py_table;

	if (PyArg_ParseTuple(args, "O", &py_table))
	{
		return NULL;
	}

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);
	uint64_t stride = cd_table_stride(table);

	return PyLong_FromUnsignedLongLong(stride);
}

PyObject *py_cd_table_count(PyObject *self, PyObject *args)
{
	PyObject *py_table;

	if (PyArg_ParseTuple(args, "O", &py_table))
	{
		return NULL;
	}

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);
	uint64_t count = cd_table_count(table);

	return PyLong_FromUnsignedLongLong(count);
}

PyObject *py_cd_table_insert(PyObject *self, PyObject *args)
{
	PyObject *py_table;
	PyObject *py_data;

	if (!PyArg_ParseTuple(args, "OO!", &py_table, &PyDict_Type, &py_data))
	{
		return NULL;
	}

	uint64_t _error = 0;

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);

	uint64_t attribute_count = PyDict_Size(py_data);
	const char **attribute_names = malloc(sizeof(*attribute_names) * attribute_count);

	uint64_t stride = 0;
	uint64_t attribute_index = 0;

	Py_ssize_t dict_pos = 0;
	PyObject *key, *value;

	while (PyDict_Next(py_data, &dict_pos, &key, &value))
	{
		if (!PyObject_TypeCheck(key, &PyUnicode_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Dictionary key needs to be a string type.");
			_error = 1;
			goto attribute_names_free;
		}

		const char *attribute_name = PyUnicode_AsUTF8(key);
		const CD_AttributeEx *attribute = cd_table_attribute_by_name(table, attribute_name);

		stride += attribute->size;

		attribute_names[attribute_index] = attribute_name;
		attribute_index++;
	}

	void *data_buffer = malloc(stride);

	dict_pos = 0;
	attribute_index = 0;
	uint64_t offset = 0;

	while (PyDict_Next(py_data, &dict_pos, &key, &value))
	{
		const char *name = PyUnicode_AsUTF8(key);
		const CD_AttributeEx *attribute = cd_table_attribute_by_name(table, name);

		void *_attrib_data = (uint8_t *)data_buffer + offset;

		switch (attribute->type)
		{
		case CD_TYPE_BYTE:
		{
			PyErr_Format(PyExc_TypeError, "Attribute '%s' is of type BYTE which hashn't been implemented yet.", name);
			_error = 1;
			goto data_buffer_free;
		}
		case CD_TYPE_UINT:
		{
			CD_uint_t *attrib_data = _attrib_data;

			if (!PyObject_TypeCheck(value, &PyLong_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type long.", name);
				_error = 1;
				goto data_buffer_free;
			}
			*attrib_data = PyLong_AsLongLong(value);
			break;
		}
		case CD_TYPE_SINT:
		{
			CD_sint_t *attrib_data = _attrib_data;

			if (!PyObject_TypeCheck(value, &PyLong_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type long.", name);
				_error = 1;
				goto data_buffer_free;
			}
			*attrib_data = PyLong_AsLongLong(value);
			break;
		}
		case CD_TYPE_FLOAT:
		{
			CD_float_t *attrib_data = _attrib_data;

			if (!PyObject_TypeCheck(value, &PyFloat_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type float.", name);
				_error = 1;
				goto data_buffer_free;
			}
			*attrib_data = PyFloat_AsDouble(value);
			break;
		}
		case CD_TYPE_CHAR:
		{
			CD_char_t *attrib_data = _attrib_data;

			if (!PyObject_TypeCheck(value, &PyUnicode_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type string.", name);
				_error = 1;
				goto data_buffer_free;
			}

			CD_char_t *string = PyBytes_AsString(PyUnicode_AsUTF8String(value));
			uint64_t length = strlen(string);
			if (length != attribute->count)
			{
				PyErr_Format(PyExc_IndexError, "Attribute '%s' needs to have length %llu but has length %llu", name, attribute->count, length);
				_error = 1;
				goto data_buffer_free;
			}

			for (uint64_t i = 0; i < length; i++)
			{
				attrib_data[i] = string[i];
			}

			break;
		}
		case CD_TYPE_VARCHAR:
		{
			CD_varchar_t *attrib_data = _attrib_data;

			if (!PyObject_TypeCheck(value, &PyUnicode_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type string.", name);
				_error = 1;
				goto data_buffer_free;
			}

			CD_varchar_t *string = PyBytes_AsString(PyUnicode_AsUTF8String(value));
			uint64_t length = strlen(string);
			if (length >= attribute->count)
			{
				PyErr_Format(PyExc_IndexError, "Attribute '%s' needs to have length smaller than %llu but has length %llu", name, attribute->count, length);
				_error = 1;
				goto data_buffer_free;
			}

			for (uint64_t i = 0; i < length; i++)
			{
				attrib_data[i] = string[i];
			}
			for (uint64_t i = length; i < attribute->count; i++)
			{
				attrib_data[i] = 0;
			}

			break;
		}
		/*case CD_TYPE_WCHAR:
		{
			CD_wchar_t *attrib_data = _attrib_data;

			if(!PyObject_TypeCheck(value, &PyUnicode_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type string.", name);
				_error = 1;
				goto data_buffer_free;
			}

			CD_wchar_t *string = PyBytes_AsString(PyUnicode_AsUTF16String(value));
			uint64_t length = wcslen(string);
			if(length != attribute->count)
			{
				PyErr_Format(PyExc_IndexError, "Attribute '%s' needs to have length %llu but has length %llu", name, attribute->count, length);
				_error = 1;
				goto data_buffer_free;
			}

			for(uint64_t i = 0; i < length; i++)
			{
				attrib_data[i] = string[i];
			}

			break;
		}
		case CD_TYPE_WVARCHAR:
		{
			CD_wvarchar_t *attrib_data = _attrib_data;

			if(!PyObject_TypeCheck(value, &PyUnicode_Type))
			{
				PyErr_Format(PyExc_TypeError, "Attribute '%s' needs to be of type string.", name);
				_error = 1;
				goto data_buffer_free;
			}

			CD_wvarchar_t *string = PyBytes_AsString(PyUnicode_AsUTF16String(value));
			uint64_t length = wcslen(string);
			if(length >= attribute->count)
			{
				PyErr_Format(PyExc_IndexError, "Attribute '%s' needs to have length smaller than %llu but has length %llu", name, attribute->count, length);
				_error = 1;
				goto data_buffer_free;
			}

			for(uint64_t i = 0; i < length; i++)
			{
				attrib_data[i] = string[i];
			}
			for(uint64_t i = length; i < attribute->count; i++)
			{
				attrib_data[i] = 0;
			}

			break;
		}*/
		default:
		{
			PyErr_Format(PyExc_TypeError, "Attribute '%s' has unknown type of value %llu", name, attribute->type);
			_error = 1;
			goto data_buffer_free;
		}
		}
		offset += cd_attribute_size(attribute->type, attribute->count);
		attribute_index++;
	}

	uint64_t success = cd_table_insert(table, attribute_count, attribute_names, data_buffer);
	if (!success)
	{
		CD_Error error = cd_get_last_error();
		PyErr_Format(PyExc_TypeError, "Failed to insert to table: %s", error.message.data);
		_error = 1;
		goto data_buffer_free;
	}

data_buffer_free:
	free(data_buffer);
attribute_names_free:
	free((void *)attribute_names);

	if (_error)
	{
		return NULL;
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject *py_cd_table_select(PyObject *self, PyObject *args)
{
	PyObject *py_table;
	PyObject *py_attribute_names;
	PyObject *py_conditions;

	if (!PyArg_ParseTuple(args, "OO!O!", &py_table, &PyList_Type, &py_attribute_names, &PyList_Type, &py_conditions))
	{
		return NULL;
	}

	uint64_t _error = 0;

	CD_Table *table = (CD_Table *)PyCapsule_GetPointer(py_table, NULL);

	uint64_t attribute_count = PyList_Size(py_attribute_names);
	const char **attribute_names = malloc(sizeof(*attribute_names) * attribute_count);

	for (uint64_t attrib_index = 0; attrib_index < attribute_count; attrib_index++)
	{
		PyObject *element = PyList_GetItem(py_attribute_names, attrib_index);

		if (!PyObject_TypeCheck(element, &PyUnicode_Type))
		{
			PyErr_SetString(PyExc_TypeError, "Attribute name needs to be a string.");
			_error = 1;
			goto attributes_free;
		}

		const char *attribute_name = PyUnicode_AsUTF8(element);
		attribute_names[attrib_index] = attribute_name;
	}

	uint64_t condition_count = PyList_Size(py_conditions);
	CD_Condition *conditions = NULL;
	if (condition_count == 0)
	{
		conditions = malloc(sizeof(*conditions) * condition_count);
		for (uint64_t i = 0; i < condition_count; i++)
		{
			conditions->data = NULL;
		}

		for (uint64_t condition_index = 0; condition_index < condition_count; condition_index++)
		{
			PyObject *element = PyList_GetItem(py_conditions, condition_index);

			if (!PyObject_TypeCheck(element, &PyDict_Type))
			{
				PyErr_SetString(PyExc_TypeError, "Condition needs to be a dictionary.");
				_error = 1;
				goto conditions_free;
			}

			CD_Condition *condition = conditions + condition_index;

			PyObject *py_name = PyDict_GetItemString(element, "name");
			if (!PyObject_TypeCheck(py_name, &PyUnicode_Type))
			{
				PyErr_SetString(PyExc_TypeError, "Condition attribute name should be a string.");
				_error = 1;
				goto conditions_free;
			}
			condition->name = PyUnicode_AsUTF8(py_name);

			PyObject *py_operator = PyDict_GetItemString(element, "operator");
			if (!PyObject_TypeCheck(py_operator, &PyLong_Type))
			{
				PyErr_SetString(PyExc_TypeError, "Condition operator should be a long.");
				_error = 1;
				goto conditions_free;
			}
			condition->operator= PyLong_AsUnsignedLongLong(py_operator);

			PyObject *py_data = PyDict_GetItemString(element, "data");

			const CD_AttributeEx *attribute = cd_table_attribute_by_name(table, condition->name);
			switch (attribute->type)
			{
			case CD_TYPE_BYTE:
			{
				PyErr_SetString(PyExc_Exception, "TYPE_BYTE not implemented yet.");
				_error = 1;
				goto conditions_free;
			}
			case CD_TYPE_UINT:
			{
				if (!PyObject_TypeCheck(py_data, &PyLong_Type))
				{
					PyErr_SetString(PyExc_TypeError, "Condition data should be an unsigned long.");
					_error = 1;
					goto conditions_free;
				}
				condition->data = malloc(sizeof(uint64_t));
				*(uint64_t *)condition->data = PyLong_AsUnsignedLongLong(py_data);
				break;
			}
			case CD_TYPE_SINT:
			{
				if (!PyObject_TypeCheck(py_data, &PyLong_Type))
				{
					PyErr_SetString(PyExc_TypeError, "Condition data should be a long.");
					_error = 1;
					goto conditions_free;
				}
				condition->data = malloc(sizeof(int64_t));
				*(int64_t *)condition->data = PyLong_AsLongLong(py_data);
				break;
			}
			case CD_TYPE_FLOAT:
			{
				if (!PyObject_TypeCheck(py_data, &PyFloat_Type))
				{
					PyErr_SetString(PyExc_TypeError, "Condition data should be a float.");
					_error = 1;
					goto conditions_free;
				}
				condition->data = malloc(sizeof(double));
				*(double *)condition->data = PyFloat_AsDouble(py_data);
				break;
			}
			case CD_TYPE_CHAR:
			{
				if (!PyObject_TypeCheck(py_data, &PyUnicode_Type))
				{
					PyErr_Format(PyExc_TypeError, "Condition data should be a string of length %llu.", attribute->count);
					_error = 1;
					goto conditions_free;
				}
				const char *string = PyUnicode_AsUTF8(py_data);
				uint64_t length = strlen(string);
				if (length != attribute->count)
				{
					PyErr_Format(PyExc_TypeError, "Condition data should be a string of length %llu.", attribute->count);
					_error = 1;
					goto conditions_free;
				}

				char *out_string = malloc(sizeof(*string) * (length + 1));
				strcpy(out_string, string);
				condition->data = out_string;
				break;
			}
			case CD_TYPE_WCHAR:
			{
				PyErr_SetString(PyExc_Exception, "TYPE_WCHAR not implemented yet.");
				_error = 1;
				goto conditions_free;
			}
			case CD_TYPE_VARCHAR:
			{
				if (!PyObject_TypeCheck(py_data, &PyUnicode_Type))
				{
					PyErr_Format(PyExc_TypeError, "Condition data should be a string of length %llu.", attribute->count);
					_error = 1;
					goto conditions_free;
				}
				const char *string = PyUnicode_AsUTF8(py_data);
				uint64_t length = strlen(string);
				if (length >= attribute->count)
				{
					PyErr_Format(PyExc_TypeError, "Condition data should have a length smaller than %llu.", attribute->count);
					_error = 1;
					goto conditions_free;
				}

				char *out_string = malloc(sizeof(*string) * (length + 1));
				strcpy(out_string, string);
				condition->data = out_string;
				break;
			}
			case CD_TYPE_WVARCHAR:
			{
				PyErr_SetString(PyExc_Exception, "TYPE_WVARCHAR not implemented yet.");
				_error = 1;
				goto conditions_free;
			}
			default:
			{
				PyErr_Format(PyExc_Exception, "Unrecognized type %llu.", attribute->type);
				_error = 1;
				goto conditions_free;
			}
			}
		}
	}

	CD_TableView *view = cd_table_select(table, attribute_count, attribute_names, condition_count, conditions);
	if (view == NULL)
	{
		CD_Error error = cd_get_last_error();
		PyErr_Format(PyExc_Exception, "Failed to select from table: %s", error.message.data);
		_error = 1;
		goto conditions_free;
	}

	PyObject *py_result = PyList_New(0);

	for (uint64_t row = 0; row < view->count_c; row++)
	{
		PyObject *dict = PyDict_New();

		for (CD_TableView_Iterator iter = cd_table_view_iterator_begin(view, row); !cd_table_view_iterator_is_end(view, row, iter); iter = cd_table_view_iterator_next(view, row, iter))
		{
			switch (iter.attribute->type)
			{
			case CD_TYPE_UINT:
			{
				const CD_uint_t value = *(const CD_uint_t *)iter.data;
				PyDict_SetItemString(dict, iter.attribute->name, PyLong_FromUnsignedLongLong(value));
				break;
			}
			case CD_TYPE_SINT:
			{
				const CD_sint_t value = *(const CD_sint_t *)iter.data;
				PyDict_SetItemString(dict, iter.attribute->name, PyLong_FromLongLong(value));
				break;
			}
			case CD_TYPE_FLOAT:
			{
				const CD_float_t value = *(const CD_float_t *)iter.data;
				PyDict_SetItemString(dict, iter.attribute->name, PyFloat_FromDouble(value));
				break;
			}
			case CD_TYPE_CHAR:
			{
				const CD_char_t *value = (const CD_char_t *)iter.data;
				PyDict_SetItemString(dict, iter.attribute->name, PyUnicode_FromStringAndSize(value, iter.attribute->count));
				break;
			}
			case CD_TYPE_VARCHAR:
			{
				const CD_varchar_t *value = (const CD_varchar_t *)iter.data;
				PyDict_SetItemString(dict, iter.attribute->name, PyUnicode_FromString(value));
				break;
			}
			default:
			{
				PyErr_Format(PyExc_Exception, "Unrecognized type %llu.", iter.attribute->type);
				_error = 1;
				goto table_view_destroy;
			}
			}
		}

		PyList_Append(py_result, dict);
	}

table_view_destroy:
	cd_table_view_destroy(view);
conditions_free:
	for (uint64_t i = 0; i < condition_count; i++)
	{
		if (conditions[i].data != NULL)
		{
			free((void *)conditions[i].data);
		}
	}
	if(conditions != NULL)
	{
		free(conditions);
	}
attributes_free:
	free(attribute_names);

	if (_error)
	{
		return NULL;
	}
	else
	{
		return py_result;
	}
}
