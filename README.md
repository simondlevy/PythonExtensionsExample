PythonExtensionsExample
==========

This repository contains a simple example of extending Python by writing C code.  The C code defines
a Python class <b>MyCircle</b> and a function <b>mysum</b>.   I have tested it in Python2 and Python3,
on Linux and Python3 on Windows (for issues associated with Windows, see 
<a href="https://stackoverflow.com/questions/2817869/error-unable-to-find-vcvarsall-bat">here</a>.)
Installing it should require only a single command in the Windows command shell
or Linux terminal window:

<ul>
<li>Windows, Python3: <b>python3 setup.py install</b>
<li>Linux, Python3: <b>sudo python3 setup.py install</b>
<li>Linux, Python2: <b>sudo python setup.py install</b>
</ul>

This command will create a new module <b>myextensions</b> in your Python libraries folder.

Once the install completes, you can test it out by running the script <b>testmyextensions.py</b>.
