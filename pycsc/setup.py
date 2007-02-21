#! /usr/bin/env python

from distutils.core import setup, Extension
import sys
import os 

description = '''Python class to communicate with smart cards'''

classifiers = ["Development Status :: 5 - Production/Stable",
"Intended Audience :: Developers",
"License :: OSI Approved :: GNU General Public License (GPL)",
"Natural Language :: English",
"Operating System :: Microsoft :: Windows",
"Operating System :: OS Independent",
"Operating System :: Unix",
"Programming Language :: C",
"Programming Language :: C++",
"Programming Language :: Python",
"Topic :: Security :: Cryptography",
"Topic :: Software Development :: Libraries :: Python Modules"]

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


setup(name="PyCSC",
    version="0.3",
    description="Python class to communicate with smart cards",
    keywords="crypto,pcsc",
    platforms="Win32 Unix",
    long_description=description,
    author="Jean-Luc Giraud",
    author_email="jlgiraud@mac.com",
    url="",
    license="BSD",
    classifiers=classifiers,
    ext_modules=[
       Extension(
           "PyCSC.pycsc",
            sources = ["pycsc.c"],
            include_dirs = include,
#            define_macros=[('DEBUG', '1')],
            undef_macros=['RELEASE'],
            library_dirs=[],
            libraries = libs,
            extra_link_args=extra_link
        )
    ],
    py_modules=["PyCSC.__init__"]
)

