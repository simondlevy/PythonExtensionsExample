#!/usr/bin/env python3
'''
testmyextensions.py test script for myextensions example

Copyright (C) 2017 Simon D. Levy

This program is part of PythonExtensionsExample

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

from myextensions import MyCircle, mysum

circle = MyCircle(4.5, 3.0, 1.2)

print(circle)

circle.move(2, 4)

print(circle)

lyst = [1,2,3]

print('Sum of ' + str(lyst) + ' = ' + str(mysum(lyst)))
