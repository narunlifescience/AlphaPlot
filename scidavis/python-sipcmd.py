#!/usr/bin/python

############################################################################
#                                                                          #
# File                 : python-sipcmd.py                                  #
# Project              : SciDAVis                                          #
# Description          : Auto-detect SIP/PyQt location; dump commandline   #
# Copyright            : (C) 2007-2010 Knut Franke (knut.franke*gmx.de)    #
#                        (replace * with @ in the email address)           #
#                                                                          #
############################################################################
#                                                                          #
#  This program is free software; you can redistribute it and/or modify    #
#  it under the terms of the GNU General Public License as published by    #
#  the Free Software Foundation; either version 2 of the License, or       #
#  (at your option) any later version.                                     #
#                                                                          #
#  This program is distributed in the hope that it will be useful,         #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
#  GNU General Public License for more details.                            #
#                                                                          #
#   You should have received a copy of the GNU General Public License      #
#   along with this program; if not, write to the Free Software            #
#   Foundation, Inc., 51 Franklin Street, Fifth Floor,                     #
#   Boston, MA  02110-1301  USA                                            #
#                                                                          #
############################################################################

from PyQt4 import pyqtconfig
config = pyqtconfig.Configuration()
flags = ["-I ../scidavis"]
if config.sip_version >= 0x040a00:
	# make use of docstring generation feature in SIP >= 4.10
	flags.append("-o")
print " ".join([config.sip_bin, "-I", config.pyqt_sip_dir, config.pyqt_sip_flags] + flags)
