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
    
    int * lidar_mm;
    
} MyClass;


static void
MyClass_dealloc(MyClass* self)
{        
    free(self->lidar_mm);
    
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
MyClass_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{    
    MyClass *self;
    
    self = (MyClass *)type->tp_alloc(type, 0);
    
    return (PyObject *)self;
}

static int
MyClass_init(MyClass *self, PyObject *args, PyObject *kwds)
{                 
    PyObject * py_laser = NULL;
    int span = 1;
    
    static char* argnames[] = {"laser", "span", NULL};

    int scan_size = 0;
    double scan_rate_hz = 0;                
    double detection_angle_degrees = 0;     
    double distance_no_detection_mm = 0;
    int detection_margin = 0;
    double offset_mm = 0;

    if(!PyArg_ParseTupleAndKeywords(args, kwds,"O|i", argnames, 
        &py_laser, 
        &span))
    {
         return error_on_raise_argument_exception("MyClass");
    }
    
    if (!int_from_obj(py_laser, "scan_size", &scan_size) ||
        !double_from_obj(py_laser, "scan_rate_hz", &scan_rate_hz) ||
        !double_from_obj(py_laser, "detection_angle_degrees", &detection_angle_degrees) ||
        !double_from_obj(py_laser, "distance_no_detection_mm", &distance_no_detection_mm) ||
        !int_from_obj(py_laser, "detection_margin", &detection_margin) ||
        !double_from_obj(py_laser, "offset_mm", &offset_mm))
    {
         return error_on_raise_argument_exception("MyClass");
    }
      
    return 0;
}


static PyObject *
MyClass_str(MyClass *self)
{        
    char str[200];
    sprintf(str, "MyClass: %d", 100);
    
    return  PyUnicode_FromString(str);
}


static PyObject *
MyClass_update(MyClass *self, PyObject *args, PyObject *kwds)
{
    PyObject * py_lidar = NULL;
    double hole_width_mm = 0;
    PyObject * py_velocities = NULL;

    static char* argnames[] = {"scans_mm", "hole_width_mm", "velocities", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds,"Od|O", argnames,
        &py_lidar, 
        &hole_width_mm,
        &py_velocities))
    {
        return null_on_raise_argument_exception("MyClass", "update");
    }

    // Bozo filter on LIDAR argument
    if (!PyList_Check(py_lidar))
    {
        return null_on_raise_argument_exception_with_details("MyClass", "update", 
            "lidar must be a list");
    }
    
    // Bozo filter on LIDAR argument list size
    if (PyList_Size(py_lidar) != 100)
    {        
        return null_on_raise_argument_exception_with_details("MyClass", "update", 
            "lidar size mismatch");
    }
    
    // Default to no velocities
    double dxy_mm = 0;
    double dtheta_degrees = 0;
          
    // Bozo filter on velocities tuple
    if (py_velocities)
    {
        if (!PyTuple_Check(py_velocities))
        {
           return null_on_raise_argument_exception_with_details("MyClass", "update", 
                "velocities must be a tuple");    
        }
        
        if (!double_from_tuple(py_velocities, 0, &dxy_mm) ||
            !double_from_tuple(py_velocities, 1, &dtheta_degrees))
            {
                return null_on_raise_argument_exception_with_details("MyClass", "update", 
                    "velocities tuple must contain at least two numbers");    
               
            }
    }
    

    // Extract LIDAR values from argument
    int k = 0;
    for (k=0; k<100; ++k)
    {
        self->lidar_mm[k] = PyFloat_AsDouble(PyList_GetItem(py_lidar, k));
    }
    
    Py_RETURN_NONE;
}


static PyMethodDef MyClass_methods[] = 
{
    {"update", (PyCFunction)MyClass_update, METH_VARARGS | METH_KEYWORDS, 
    "MyClass.update(scans_mm, hole_width_mm, velocities=None) updates scan.\n"\
    "scans_mm is a list of integers representing scanned distances in mm.\n"\
    "hole_width_mm is the width of holes (obstacles, walls) in millimeters.\n"\
    "velocities is an optional tuple containing at least dxy_mm, dtheta_degrees;\n"\
    "i.e., robot's (forward, rotational velocity) for improving the quality of the scan."
    },
    {NULL}  // Sentinel 
};

#define TP_DOC_SCAN \
"A class for Lidar scans.\n" \
"MyClass.__init__(laser, span=1)\n"\
    "laser is a Laser object containing parameters of your laser rangefinder (Lidar)\n"\
"    span supports spanning laser scan to cover the space better"


static PyTypeObject myextensions_MyClassType = 
{
    #if PY_MAJOR_VERSION < 3
    PyObject_HEAD_INIT(NULL)
    0,                                          // ob_size
    #else
    PyVarObject_HEAD_INIT(NULL, 0)
    #endif
    "myextensions.MyClass",                      // tp_name
    sizeof(MyClass),                               // tp_basicsize
    0,                                          // tp_itemsize
    (destructor)MyClass_dealloc,                   // tp_dealloc
    0,                                          // tp_print
    0,                                          // tp_getattr
    0,                                          // tp_setattr
    0,                                          // tp_compare
    (reprfunc)MyClass_str,                         // tp_repr
    0,                                          // tp_as_number
    0,                                          // tp_as_sequence
    0,                                          // tp_as_positionping
    0,                                          // tp_hash 
    0,                                          // tp_call
    (reprfunc)MyClass_str,                         // tp_str
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
    MyClass_methods,                         		// tp_methods 
    0,                         					// tp_members 
    0,                                          // tp_getset 
    0,                                          // tp_base 
    0,                                          // tp_dict 
    0,                                          // tp_descr_get 
    0,                                          // tp_descr_set 
    0,                                          // tp_dictoffset 
    (initproc)MyClass_init,                        // tp_init 
    0,                                          // tp_alloc 
    MyClass_new,                                   // tp_new 
};



// myextensions module ------------------------------------------------------------


static PyObject *
distance(PyObject *self, PyObject *args)
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

