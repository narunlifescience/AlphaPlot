#################################################
# SciDAVis main project file
# run "qmake scidavis.pro" to generate the Makefile
# In most cases you don't have to make changes to
# this file but to the "scidavis.pro" file in the
# "scidavis" subdirectory.
#################################################
TEMPLATE = subdirs

SUBDIRS = 3rdparty/liborigin scidavis test
CONFIG+=ordered

# Overwrite the qmake generated Makefile by the cmake one
mxe {
# urrgh! - maybe adding a qmake.pro file for liborigin might be better...
  system("cd 3rdparty/liborigin; rm CMakeCache.txt; cmake  -DCMAKE_TOOLCHAIN_FILE=/scratch/rks/github/mxe/usr/i686-pc-mingw32.static/share/cmake/mxe-conf.cmake  -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_CXX_FLAGS:STRING=-DNO_CODE_GENERATION_FOR_LOG .")
} else {
  system("cd 3rdparty/liborigin; rm CMakeCache.txt; cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_CXX_FLAGS:STRING=-DNO_CODE_GENERATION_FOR_LOG .")
}

!mxe {
# optional fit plugins
SUBDIRS += fitPlugins/fitRational0\
           fitPlugins/fitRational1\
           fitPlugins/explin\
           fitPlugins/exp_saturation\
           fitPlugins/planck_wavelength\
}
