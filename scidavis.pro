# SciDAVis main project file
# run "qmake scidavis.pro" to generate the Makefile
TEMPLATE = subdirs

SUBDIRS = scidavis

# optional fit plugins
SUBDIRS += fitPlugins/fitRational0\
           fitPlugins/fitRational1
