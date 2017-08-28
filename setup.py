#!/usr/bin/env python

'''
setup.py - Python distutils setup file for PythonExtensionsExample

Copyright (C) 2017 Simon D. Levy

This program is part of PythonExtensions Example

PythonExtensionsExample is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

PythonExtensionsExample is distributed in the hope that it will be useful,     
but WITHOUT ANY WARRANTY without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License 
along with this code.  If not, see <http://www.gnu.org/licenses/>.
'''

# Support streaming SIMD extensions

from platform import machine

OPT_FLAGS  = []
SIMD_FLAGS = []

SOURCES = [
    'myextensions.c', 
    'pyextension_utils.c']

from distutils.core import setup, Extension

module = Extension('myextensions', 
    sources = SOURCES, 
    extra_compile_args = SIMD_FLAGS + OPT_FLAGS
    )


setup (name = 'PythonExtensionsExample',
    version = '0.1',
    description = 'An example of how to write Python extensions in C',
    ext_modules = [module],
    author='Simon D. Levy',
    author_email='simon.d.levy@gmail.com',
    url='http://github.com/simondlevy/PythonExtensionsExample',
    license='LGPL',
    platforms='Linux; Windows; OS X',
    long_description = 'Shows how to create a class and a standalone function'
    )
