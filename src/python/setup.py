#This file uses generator expression from CMake to set up all the paths and
#files. The general framework would work for manual use but the expression would
# need to be replaced with equivalent values.

from setuptools import Extension, setup
import platform
import os

#TODO I think this can be replaced by some Cmake
def get_extra_compile_args():
    if platform.system() == 'Windows':
        return ["/std:c++17"]
    else:
        return ["-std=c++17"]

def get_extra_link_args():
    if platform.system() == 'Windows':
        return []
    else:
        return ["-Wl,-rpath,$ORIGIN/lib/."]

# read the contents of your README file
from pathlib import Path
this_directory = Path(__file__).parent
long_description = (this_directory / "README.md").read_text()

#Nominally this is just one directory. This just  ensures it doesn't have to be.
incl_dirs    = "$<TARGET_PROPERTY:saline-cpp,INCLUDE_DIRECTORIES>".split(";")
incl_dirs.extend("$<TARGET_PROPERTY:SalinePy,INTERFACE_INCLUDE_DIRECTORIES>".split(";"))
ext_obj_list = "$<TARGET_OBJECTS:saline-cpp>".split(";")
link_lbs     = "$<TARGET_PROPERTY:saline-cpp,LINK_LIBRARIES>".split(";")
srcs         = [src for src in "$<TARGET_PROPERTY:saline-cpp,SOURCES>".split(";") if src and not src.strip().endswith('.natvis')]

print(srcs)

ext_mod = Extension("_SalinePy",
        #Source files for wheel
        sources            = ["SalinePy_wrap.cxx"] +srcs,
        #Header files for the Saline C++ code
        include_dirs       = incl_dirs,
        extra_compile_args = get_extra_compile_args(),
        #Arguments for helping the linker and runtime loader
        extra_link_args    = get_extra_link_args(),
        py_limited_api   = True,
        )

setup(
        name            = "SalinePy",
        version         = "$<TARGET_PROPERTY:saline,VERSION>",
        python_requires = ">=3.8, <4",
        py_modules      = ["SalinePy"],
        author          = "Shane C. Henderson",
        author_email    = "hendersonsc@ornl",
        url             = "https://code.ornl.gov/NEAMS/saline",
        long_description= long_description,
        long_description_content_type="text/markdown",
        ext_modules     = [ext_mod],
        license_files   = "LICENSE",
    )
