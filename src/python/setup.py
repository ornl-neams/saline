#This file uses generator expression from CMake to set up all the paths and
#files. The general framework would work for manual use but the expression would
# need to be replaced with equivalent values.

import itertools
from setuptools import Extension, setup
from distutils.command.build import build as build_orig

incl_dirs = "$<TARGET_PROPERTY:saline-cpp,INCLUDE_DIRECTORIES>".split(";")
ext_obj_list = "$<TARGET_OBJECTS:saline-cpp>".split(";")

ext_mod = Extension("_SalinePy",
        sources            = ["SalinePy_wrap.cxx"],
        include_dirs       = incl_dirs,
        extra_objects      = ext_obj_list,
#Technically this should be overridden with the compile options used by cmake...
        extra_compile_args = ["-std=c++11"],
        )

setup(
        name            = "SalinePy",
        python_requires = ">3.8",
        version         = "0.0.1",
        ext_modules     = [ext_mod],
        py_modules      = ["SalinePy"],
    )
