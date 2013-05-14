# Patch Qwt and Qwtplot3D configurations to fit into SciDAVis/Windows build process

qwt_conf = "qwt/qwtconfig.pri"
qwtplot3d_conf = "qwtplot3d/qwtplot3d.pro"

# patch Qwt config file
with open(qwt_conf, 'r') as file:
  content = file.readlines()
with open (qwt_conf, 'w') as file:
  for line in content:
    if ("QwtDll" in line or "QwtWidgets" in line or "QwtDesigner" in line) and line[0] != '#':
      file.write("#")
    file.write(line)

# patch Qwtplot3D config file
with open(qwtplot3d_conf, 'r') as file:
  content = file.readlines()
with open(qwtplot3d_conf, 'w') as file:
  for line in content:
    if ("vclib" in line or "DESTDIR" in line) and line[0] != '#':
      file.write("#")
    file.write(line.replace("debug", "release"))
  file.write("\nDESTDIR = ../../output\n")
