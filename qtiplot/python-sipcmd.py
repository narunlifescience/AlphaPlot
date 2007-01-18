#!/usr/bin/python
from distutils import sysconfig
import os.path
cmd = os.path.join(sysconfig.EXEC_PREFIX, 'sip.exe')
o_inc = '-I ' + os.path.join(sysconfig.PREFIX, 'sip', 'PyQt4')
o_qtver = '-t Qt_4_2_1'
print cmd, o_inc
