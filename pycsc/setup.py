#!/usr/bin/python

#***********************************************************
#        Title: setup.py
#       Author: Jean-Luc Giraud <jlgiraud@mac.com>
#     Compiler: python2.2
#     Platform: Linux
#      Purpose: Python class to communicate with smart cards
#      License: See file COPYING
#          $Id: setup.py,v 1.4 2004-01-19 16:09:32 giraud Exp $
#
#      Copyright (c) 2002 - Jean-Luc Giraud.
#
#************************************************************



from distutils.core import setup, Extension
import sys
import os 

if sys.platform == 'darwin':

    extra_link = ['-framework', 'PCSC']
    lib_dirs   = ["/System/Library/Frameworks/PCSC.framework/"]
    libs       = []
    include    = ["/System/Library/Frameworks/PCSC.framework/Headers/"]
else:
    extra_link = []
    lib_dirs   = []
    if os.name == 'nt':
         libs  = ["winscard"]
         include = []
    else:
         libs = ["pcsclite"]
         include = ["/usr/include/PCSC"]


setup(name="pycsc", version="0.3",
   ext_modules=[
       Extension(
           "pycsc",
            ["pycsc.c"],
            include_dirs = include,
#            define_macros=[('DEBUG', '1')],
            undef_macros=['RELEASE'],
            library_dirs=[],
            libraries = libs,
            extra_link_args=extra_link
        )
    ]
)
