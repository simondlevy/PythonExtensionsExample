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
#include <math.h>
#include "pyextension_utils.h"

// MyCircle class ------------------------------------------------------------

typedef struct 
{
    PyObject_HEAD

    double x;
    double y;
    double r;

} MyCircle;


static void MyCircle_dealloc(MyCircle* self)
{        
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * MyCircle_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{    
    MyCircle *self;

    self = (MyCircle *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static int MyCircle_init(MyCircle *self, PyObject *args, PyObject *kwds)
{                 
    static char* argnames[] = {"x", "y", "r", NULL};

    if(!PyArg_ParseTupleAndKeywords(args, kwds,"ddd", argnames, 
                &self->x, &self->y, &self->r)) {
        return error_on_raise_argument_exception("MyCircle");
    }

    return 0;
}


static PyObject * MyCircle_str(MyCircle *self)
{        
    char str[200];
    sprintf(str, "Circle at (%2.2f,%2.2f) with radius %2.2f", self->x, self->y, self->r);

    return  PyUnicode_FromString(str);
}


static PyObject * MyCircle_move(MyCircle *self, PyObject *args, PyObject *kwds)
{
    static char* argnames[] = {"dx", "dy", NULL};
    double dx = 0, dy = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds,"dd", argnames, dx, dy)) {
        return null_on_raise_argument_exception("MyCircle", "move");
    }

    self->x += dx;
    self->y += dy;

    Py_RETURN_NONE;
}


static PyObject * MyCircle_area(MyCircle *self, PyObject *args, PyObject *kwds)
{
    return PyFloat_FromDouble(M_PI*self->r*self->r);
}


static PyMethodDef MyCircle_methods[] = 
{
    {"move", (PyCFunction)MyCircle_move, METH_VARARGS | METH_KEYWORDS, 
     "MyCircle.move(dx,dy) changes position.\n"\
    },
    {"area", (PyCFunction)MyCircle_area, METH_VARARGS | METH_KEYWORDS, 
     "MyCircle.area() returns area.\n"\
    },
    {NULL}  // Sentinel 
};

#define TP_DOC_SCAN \
"An example class.\n" \
"MyCircle.__init__(value)\n"\
"Creates a MyCircle object with specified value"


static PyTypeObject myextensions_MyCircleType = 
{
#if PY_MAJOR_VERSION < 3
    PyObject_HEAD_INIT(NULL)
        0,                                      // ob_size
#else
    PyVarObject_HEAD_INIT(NULL, 0)
#endif
    "myextensions.MyCircle",                      // tp_name
    sizeof(MyCircle),                             // tp_basicsize
    0,                                          // tp_itemsize
    (destructor)MyCircle_dealloc,                 // tp_dealloc
    0,                                          // tp_print
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_compare
    (reprfunc)MyCircle_str,                       // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_positionping
    0,                                          // tp_hash 
    0,                                          // tp_call
    (reprfunc)MyCircle_str,                       // tp_str
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
    MyCircle_methods,                         	// tp_methods 
    0,                         					// tp_members 
    0,                                          // tp_getset 
    0,                                          // tp_base 
    0,                                          // tp_dict 
    0,                                          // tp_descr_get 
    0,                                          // tp_descr_set 
    0,                                          // tp_dictoffset 
    (initproc)MyCircle_init,                      // tp_init 
    0,                                          // tp_alloc 
    MyCircle_new,                                 // tp_new 
};



// myextensions module ------------------------------------------------------------


static PyObject * mysum(PyObject *self, PyObject *args)
{   
    MyCircle * py_scan = NULL;

    // Extract Python objects for map, scan, and position
    if (!PyArg_ParseTuple(args, "O", &py_scan))
    {        
        return null_on_raise_argument_exception("breezyslam", "mysum");
    }

    // Check object types
    if ( error_on_check_argument_type((PyObject *)py_scan, &myextensions_MyCircleType, 1,
                "myextensions.MyCircle", "myextensions", "mysum")) {

        return NULL;
    }

    // Run C version and return Python integer
    return PyLong_FromLong(100);
}

static PyMethodDef module_methods[] = 
{
    {"mysum", mysum, METH_VARARGS,
     "mysum(lyst)\n"\
     "Returns sum of numbers in list lyst."
    },
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static void add_classes(PyObject * module)
{
    add_class(module, &myextensions_MyCircleType, "MyCircle");
}

static int types_are_ready(void)
{
    return type_is_ready(&myextensions_MyCircleType);
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

