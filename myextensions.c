/*
   myextensions.c : C extensions example for Python

   Copyright (C) 2017 Simon D. Levy

   This code is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as 
   published by the Free Software Foundation, either version 3 of the 
   License, or (at your option) any later version.

   This code is distributed in the hope that it will be useful,     
   but WITHOUT ANY WARRANTY without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with this code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Python.h>
#include <structmember.h>

#include "pyextension_utils.h"

// MyClass class ------------------------------------------------------------

typedef struct 
{
    PyObject_HEAD

    int value;

} MyClass;


static void MyClass_dealloc(MyClass* self)
{        
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * MyClass_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{    
    MyClass *self;

    self = (MyClass *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static int MyClass_init(MyClass *self, PyObject *args, PyObject *kwds)
{                 
    static char* argnames[] = {"value", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwds,"i", argnames, &self->value)) {
        return error_on_raise_argument_exception("MyClass");
    }

    return 0;
}


static PyObject * MyClass_str(MyClass *self)
{        
    char str[200];
    sprintf(str, "value = %d", self->value);

    return  PyUnicode_FromString(str);
}


static PyObject * MyClass_set(MyClass *self, PyObject *args, PyObject *kwds)
{
    static char* argnames[] = {"value", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds,"i", argnames, self->value)) {
        return null_on_raise_argument_exception("MyClass", "update");
    }

    Py_RETURN_NONE;
}


static PyObject * MyClass_get(MyClass *self, PyObject *args, PyObject *kwds)
{
    return PyLong_FromLong(self->value);
}


static PyMethodDef MyClass_methods[] = 
{
    {"set", (PyCFunction)MyClass_set, METH_VARARGS | METH_KEYWORDS, 
     "MyClass.set(value) changes value.\n"\
    },
    {"get", (PyCFunction)MyClass_get, METH_VARARGS | METH_KEYWORDS, 
     "MyClass.get() returns value.\n"\
    },
    {NULL}  // Sentinel 
};

#define TP_DOC_SCAN \
"An example class.\n" \
"MyClass.__init__(value)\n"\
"Creates a MyClass object with specified value"


static PyTypeObject myextensions_MyClassType = 
{
#if PY_MAJOR_VERSION < 3
    PyObject_HEAD_INIT(NULL)
        0,                                      // ob_size
#else
    PyVarObject_HEAD_INIT(NULL, 0)
#endif
    "myextensions.MyClass",                      // tp_name
    sizeof(MyClass),                             // tp_basicsize
    0,                                          // tp_itemsize
    (destructor)MyClass_dealloc,                 // tp_dealloc
    0,                                          // tp_print
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_compare
    (reprfunc)MyClass_str,                       // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_positionping
    0,                                          // tp_hash 
    0,                                          // tp_call
    (reprfunc)MyClass_str,                       // tp_str
    0,                                          // tp_getattro
    0,                                          // tp_setattro
    0,                                          // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   // tp_flags
    TP_DOC_SCAN,                                // tp_doc 
    0,                                          // tp_traverse 
    0,                                          // tp_clear 
    0,                                          // tp_richcompare 
    0,                                          // tp_weaklistoffset 
    0,                                          // tp_iter 
    0,                                          // tp_iternext 
    MyClass_methods,                         	// tp_methods 
    0,                         					// tp_members 
    0,                                          // tp_getset 
    0,                                          // tp_base 
    0,                                          // tp_dict 
    0,                                          // tp_descr_get 
    0,                                          // tp_descr_set 
    0,                                          // tp_dictoffset 
    (initproc)MyClass_init,                      // tp_init 
    0,                                          // tp_alloc 
    MyClass_new,                                 // tp_new 
};



// myextensions module ------------------------------------------------------------


static PyObject * distance(PyObject *self, PyObject *args)
{   
    MyClass * py_scan = NULL;

    // Extract Python objects for map, scan, and position
    if (!PyArg_ParseTuple(args, "O", &py_scan))
    {        
        return null_on_raise_argument_exception("breezyslam", "distance");
    }

    // Check object types
    if ( error_on_check_argument_type((PyObject *)py_scan, &myextensions_MyClassType, 1,
                "myextensions.MyClass", "myextensions", "distance")) {

        return NULL;
    }

    // Run C version and return Python integer
    return PyLong_FromLong(100);
}

static PyMethodDef module_methods[] = 
{
    {"distance", distance, METH_VARARGS,
        "distance(scan)\n"
            "Computes distance between a scan and map, given hypothetical position, to support particle filtering.\n"\
            "Returns -1 for infinity.\n"\
            "scan is a breezyslam.components.MyClass object\n"\
    },
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static void add_classes(PyObject * module)
{
    add_class(module, &myextensions_MyClassType, "MyClass");
}

static int types_are_ready(void)
{
    return type_is_ready(&myextensions_MyClassType);
}

#if PY_MAJOR_VERSION < 3

    PyMODINIT_FUNC
initmyextensions(void) 
{    
    if (!types_are_ready())    {
        return;
    }

    PyObject * module = Py_InitModule("myextensions", module_methods);

    if (module == NULL)
    {
        return;
    }

    add_classes(module);    
}

#else

static PyModuleDef moduledef = 
{
    PyModuleDef_HEAD_INIT,
    "myextensions",
    "Python Extensions Example module",  
    -1,                 // m_size
    module_methods,     
    NULL,               // m_reload
    NULL,               // m_traverse
    NULL,               // m_clear
    NULL                // m_free
};

    PyMODINIT_FUNC
PyInit_myextensions(void) 
{    
    if (!types_are_ready())
    {
        return NULL;
    }

    PyObject* module = PyModule_Create(&moduledef);

    if (module == NULL)
    {
        return NULL;
    }

    add_classes(module);

    return module;
}

#endif

