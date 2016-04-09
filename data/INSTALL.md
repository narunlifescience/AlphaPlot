# AlphaPlot - Installation Notes

**Please visit our [homepage](http://AlphaPlot.sourceforge.net) for more information.**

## Generic requirements
In order to compile AlphaPlot, you need to install the following libraries:
- [Qt for C++ (>=4.3)](http://www.qtsoftware.com/downloads/opensource/appdev)
- [Qwt (>=5.1.0)](http://qwt.sourceforge.net/index.html)
- [QwtPlot3D (>=0.2.6)](http://qwtplot3d.sourceforge.net/)
- [GSL (>=1.8)](http://www.gnu.org/software/gsl/)
- [muParser (>=1.28)](http://muparser.sourceforge.net)
- [zlib (>=1.2.3)](http://www.zlib.net/)

**QwtPlot3D and Qwt must be compiled against Qt 4.x! If these are compiled with Qt3, the application will crash!** See below for how to ensure this.

## Linux

1.  Install [Qt (>=4.3)](http://www.qtsoftware.com/downloads/opensource/appdev/linux-x11-cpp), [GSL](http://sources.redhat.com/gsl/), [muParser](http://muparser.sourceforge.net) and [zlib](http://www.zlib.net/); either from your package manager or from the sources. If you are using precompiled packages, make sure you also install the corresponding development packages (*-dev or *-devel). If you want to display the manual from within AlphaPlot (you can also view it with your favorite browser), you need to have the Qt Assistant installed, which sometimes comes in a separate package (qt4-dev-tools for Ubuntu edgy).
2.  Open a terminal window.
3.  Get [Qwt (>=5.1.0)](http://qwt.sourceforge.net/index.html) sources and unpack them to the 3rdparty folder:  
     `cd AlphaPlot-X.X.X/3rdparty  
    tar -xvjf /path/to/qwt-Y.Y.Y.tar.bz2`
4.  Rename the new directory to `qwt`:  
     `mv qwt-Y.Y.Y qwt`
5.  We want to build the static version of Qwt in order to avoid potential conflicts with an already installed version. Open the file qwt/qwtconfig.pri and comment out the line  
     `CONFIG += QwtDll`
6.  Build Qwt:  
    `cd qwt; qmake && make; cd ..`  
     On some systems (such as Debian, Ubuntu, ...) the `qmake`-Tool for Qt 4.x is called `qmake-qt4`. Make sure you don't use the version that belongs to Qt 3.x.
7.  Get [QwtPlot3D](http://qwtplot3d.sourceforge.net/) sources and also unpack them to AlphaPlot-X.X.X/3rdparty:  
     `tar -xvzf qwtplot3d-Z.Z.Z.tgz`
8.  We also want to build a static version here, so open qwtplot3d/qwtplot3d.pro in your favorite text editor and add the line  
     `CONFIG += staticlib`  
     at a convenient place (say, below the other CONFIG line).
9.  Build qwtplot3d:  
    `cd qwtplot3d; qmake && make; cd ..`
10.  Go to the directory AlphaPlot-X.X.X/AlphaPlot:  
    `cd ../AlphaPlot`
11.  If you want to use Python expressions and scripts, make sure you have the following additional dependencies installed: [Python](http://www.python.org/) (>=2.5), [SIP](http://www.riverbankcomputing.co.uk/software/sip/intro) (>=4.6), [PyQt](http://www.riverbankcomputing.co.uk/software/pyqt/intro) (>=4.2). Other versions as those indicated above may or may not work. Specifically, there are known issues with a number of SIP versions and a given version of PyQt will only work with a limited range of SIP versions.   
    Open AlphaPlot.pro in a text editor and remove the comment marker (#) from the line `include( python.pri )`.
12.  Build AlphaPlot:  
    `qmake && make`
13.  You should now be able to run AlphaPlot by entering  
    `./AlphaPlot`
14.  Optionally, install AlphaPlot on your system (you need root priviliges):  
     `sudo make install`  
    or, alternatively,  
    `su -c "make install"`

## Windows

1.  Download the "Qt/Windows Open Source Edition" from [http://www.qtsoftware.com/downloads/opensource/appdev/windows-cpp](http://www.qtsoftware.com/downloads/opensource/appdev/windows-cpp). It comes with a graphical installer. When the Qt installer asks you to install MinGW, say yes unless you already have it on your system. In that case you have to tell the Qt installer where to find it. In the case that PyQt (see blow) does not yet support the latest version of Qt, you can get older versions here: [ftp://ftp.trolltech.com/qt/source/](ftp://ftp.trolltech.com/qt/source/).
2.  Download Python from [http://www.python.org/download/](http://www.python.org/download/) and install it.
3.  Download the source code of AlphaPlot from [http://AlphaPlot.sourceforge.net/download.html](http://AlphaPlot.sourceforge.net/download.html) and unpack it.
4.  Download the SIP source code for Windows from [http://www.riverbankcomputing.co.uk/software/sip/download](http://www.riverbankcomputing.co.uk/software/sip/download) and unpack it into the 3rdparty subfolder of the AlphaPlot package. Rename the "sip-x.y" directory that gets created to "sip".
5.  Download the PyQt v4 source code for Windows from [http://www.riverbankcomputing.co.uk/software/pyqt/download](http://www.riverbankcomputing.co.uk/software/pyqt/download) and unpack it into the 3rdparty subfolder of the AlphaPlot package. Rename the "PyQt-win-gpl-x.y.z" directory that gets created to "PyQt-win-gpl".
6.  Download the "Developer files" of the GSL from [http://gnuwin32.sourceforge.net/packages/gsl.htm](http://gnuwin32.sourceforge.net/packages/gsl.htm) and unpack them into the 3rdparty subfolder of the AlphaPlot package.
7.  Download the source code of the latest version of Qwt from [http://sourceforge.net/project/showfiles.php?group_id=13693](http://sourceforge.net/project/showfiles.php?group_id=13693) and unpack it into the 3rdparty subfolder of the AlphaPlot package. Rename the "qwt-x.y.z" directory that gets created to "qwt".
8.  Download the source code of QwtPlot3D from [http://qwtplot3d.sourceforge.net/web/navigation/download_frame.html](http://qwtplot3d.sourceforge.net/web/navigation/download_frame.html) and unpack it into the 3rdparty subfolder of the AlphaPlot package.
9.  Download the source code of muParser from [http://sourceforge.net/project/showfiles.php?group_id=137191](http://sourceforge.net/project/showfiles.php?group_id=137191) and unpack it into the 3rdparty subfolder of the AlphaPlot package. Rename the "muparser_vxyz" directory that gets created to "muparser".
10.  Open the start menu, find the folder created by the Qt SDK and start a "Qt Command Prompt". Execute "cd <directory containing AlphaPlot sources>", then "build". AlphaPlot and its dependencies should be built automatically and installed into the directory "output".

## Windows(cross compile on linux using MXE)

MXE is a version of MinGW that has been compiled on Linux and acts as a cross-compilation environment targeting the Windows operating system. More details can be found at http://mxe.cc/

1. Install all dependencies required for MXE according to your linux distro as described here http://mxe.cc/#requirements
2. download current version of MXE(lets use the home directory $HOME)

    `cd ~`
    `git clone https://github.com/mxe/mxe.git`
    `cd mxe`

3. Install dependencies required by Alphaplot(this may take some time)

    `make qt gsl zlib muparser`

4. Create a symbolic link of MXE version of qmake(qt4) & minGW and copy it to /bin directory     
Note:Qt 4 is in the "qt" subdirectory. Qt 5 is in the "qt5" subdirectory (use QT4)

    `sudo ln -sf ~/mxe/usr/i686-w64-mingw32.static/qt/bin/qmake /bin/mxe-qmake-qt4`
    `sudo ln -sf ~/mxe/usr/BIN/i686-w64-mingw32.static-g++ /bin/i686-w64-mingw32.static-g++`
    `sudo ln -sf ~/mxe/usr/BIN/i686-w64-mingw32.static-gcc-ar /bin/i686-w64-mingw32.static-gcc-ar`

5. download QWT5 sources from https://sourceforge.net/projects/qwt/files/qwt/5.2.1/, unpack, edit qwtconfig.pri & comment out the following lines:

    `#CONFIG += thread`
    `#CONFIG += QwtDll`

6. open terminal, get in to qwt5 directory & start build

    `mxe-qmake-qt4`
    `make`

7. download qwtplot3d sources from https://sourceforge.net/projects/qwtplot3d/, unpack, edit qwtplot3d.pro & comment out the following lines:

    `#win32:TEMPLATE    = vclib`
    `#win32:CONFIG     += dll exceptions`
    `#win32:dll:DEFINES    += QT_DLL QWT3D_DLL QWT3D_MAKEDLL`

Note: patches may be required.. for eg arch based systems may need https://projects.archlinux.org/svntogit/packages.git/tree/trunk/qwtplot3d-gcc44.patch?h=packages/qwtplot3d & https://projects.archlinux.org/svntogit/packages.git/tree/trunk/qwtplot3d-qt-4.8.0.patch?h=packages/qwtplot3d

8. open terminal, get in to qwtplot3d directory & start build

    `mxe-qmake-qt4`
    `make`

9. download latest sources of AlphaPlot(lets use the home directory $HOME)

    `cd ~`
    `git clone https://github.com/narunlifescience/AlphaPlot.git`

10. create include & lib directory inside ~/AlphaPlot/3rdparty/
11. copy contents of src directory(only *.h files) inside qwtplot3d to ~/AlphaPlot/3rdparty/include
12. copy contents of include directory inside qwtplot3d to ~/AlphaPlot/3rdparty/include
13. copy lib/libqwt.a inside qwt to ~/AlphaPlot/3rdparty/lib
14. copy lib/libqwtplot3d.a inside qwtplot3d to ~/AlphaPlot/3rdparty/lib
15. now we can build AlphaPlot
    
    `cd ~/AlphaPlot`
    `mxe-qmake-qt4`
    `make`

After build process you will get AlphaPlot.exe inside ~/AlphaPlot/AlphaPlot directory...

## Mac OS X

These instructions where kindly provided by Yu-Hung Lien.

**Downloading the requirements:**  
 Please download Qt, GSL, MuParser, Qwt and Qwt3D from their web sites before building:  

1.  Qt: [ftp://ftp.trolltech.com/qt/source/qt-mac-opensource-4.4.1.dmg](ftp://ftp.trolltech.com/qt/source/qt-mac-opensource-4.4.1.dmg) (this is for version 4.4.1, the current one at the time of writing, there may be a newer)  

2.  GSL: [ftp://ftp.gnu.org/gnu/gsl/](ftp://ftp.gnu.org/gnu/gsl/) (the current version at the time of writing: 1.11)
3.  muParser: [http://downloads.sourceforge.net/muparser/muparser_v130.tar.gz?modtime=1219351493&big_mirror=0"](http://downloads.sourceforge.net/muparser/muparser_v130.tar.gz?modtime=1219351493&big_mirror=0) (the current version at the time of writing: 1.30)
4.  Qwt: [http://downloads.sourceforge.net/qwt/qwt-5.1.1.zip?modtime=1211655872&big_mirror=0](http://downloads.sourceforge.net/qwt/qwt-5.1.1.zip?modtime=1211655872&big_mirror=0) (the current version at the time of writing: 5.1.1)
5.  QwtPlot3D: [http://prdownloads.sourceforge.net/qwtplot3d/qwtplot3d-0.2.7.zip?download](http://prdownloads.sourceforge.net/qwtplot3d/qwtplot3d-0.2.7.zip?download) (the current version at the time of writing: 0.2.7)

**(I) Building the OS X binary on one's own platform:**  

1.  Open the image file and install Qt for Mac.
2.  Open Terminal and change to the directory which the gsl-x.xx.tar.gz locates. For example, type "cd Downloads", if gsl-x.xx.tar.gz locates in the downloads folder of your home directory.
3.  Type " tar -xzf gsl-x.xx.tar.gz" to extract the files
4.  Type "cd gsl-x.xx"
5.  Type "./configure".
6.  Type " sudo make" and enter your password as prompted.
7.  If there is no error occurring, then type "sudo make install" to install GSL. If you regret for the installation, you can type "sudo make uninstall". If there is any error occurs and you want to restart the process, please type "make clean".
8.  Change to the directory which the muparser\*.tar.gz locates and type " tar -xzf muparser\*.tar.gz" to extract the files.
9.  Type "cd muparser".
10.  Type "./configure --enable-shared=no" because AlphaPlot requires static library.
11.  Type "make".
12.  Type "sudo make install". The same options of make in (7) are still applicable.
13.  Change to the directory which the qwt\*.zip locates and type "unzip qwt\*.zip" to extract the files.
14.  Change to the directory which the qwtplot3d\*.zip locates and type "unzip qwtplot3d\*.zip" to extract the files.
15.  Change to the directory which the AlphaPlot\*.zip locates and type "unzip AlphaPlot\*.zip" to extract the files.
16.  Copy both qwt-\*.\*.\* and qwtplot3d folders into `AlphaPlot_*/3rdparty/`, and change the folder name qwt-\*.\*.\* into qwt
17.  Change to the newly created directory `AlphaPlot*/3rdparty/qwt`
18.  Open qwtconfig.pri with any text editor and add "#" at the beginning of the line "CONFIG += QwtDll" for building static library.
19.  Type "qmake -spec macx-g++" to create make file for gcc compilier instead of xcode.
20.  Type "make". The same option "clean" of make in (7) is still applicable here.
21.  Change to the newly created directory `AlphaPlot*/3rdparty/qwtplot3d`
22.  Open qwtplot3d.pro with any text editor and add a line "CONFIG += staticlib" next to the line "QT += opengl" for building static library.
23.  Type "qmake -spec macx-g++" to create make file for gcc compilier instead of xcode.
24.  Type "make". The same option "clean" of make in (7) is still applicable here.
25.  Change to the directory `AlphaPlot*/` and type `qmake -spec macx-g++`
26.  Type `sudo make`
27.  As the building ends up with no error, the AlphaPlot application will be in `AlphaPlot*/AlphaPlot`. Unfortunately, the icon seems missed for some unknown reason and shows as a generic application icon.

**(II) Building Universal Binaries (UB):**  
 The whole process is essentially same as above except some extra commands for universal binaries. The references for building universal binaries for typical unix make systems can be found at:  

1.  [Porting UNIX/Linux Applications to Mac OS X: Compiling for Multiple CPU Architectures](http://developer.apple.com/documentation/Porting/Conceptual/PortingUnix/compiling/chapter_4_section_3.html#//apple_ref/doc/uid/TP40002850-BAJCFEBA)
2.  [Building Universal Binaries from "configure"-based Open Source Projects](http://developer.apple.com/technotes/tn2005/tn2137.html)
3.  [qmake Platform Notes](http://doc.trolltech.com/4.4/qmake-platform-notes.html#mac-os-x)
4.  [Deploying an Application on Qt/Mac](http://doc.trolltech.com/4.4/deployment-mac.html)

The basic step for building UB is to pass the option "-arch ppc -arch i386" to compiler gcc and linker ld. Qt simply takes care this problem by adding the block  
 `macx {  
 CONFIG += x86 ppc  
 }` in each source's `*.pro` file. However, it is somewhat different for unix "configure"-base stuffs. The followings are the procedure for reference.  

1.  Building GSL in UB

1.  Change to the root directory of GSL source and type "./configure --disable-dependency-tracking"
2.  Type "make CFLAGS="-g -O2 -arch i386 -arch ppc" LDFLAGS="-arch i386 -arch ppc"" to pass the desired option. The original values of CFLAGS and LDFLAGS in makefile after executing configure script are "-g -O2" and "", respectively. Therefore, I just append "-arch i386 -arch ppc" to the original values and put them after "make" to override.
3.  Same as (I).(7).

3.  Building muParser in UB

1.  Change to the root directory of GSL source and type `./configure --enable-shared=no --disable-dependency-tracking`.
2.  Type `make CPPFLAGS="-arch i386 -arch ppc" LDFLAGS="-arch i386 -arch ppc"`. I do not use CFLAGS variable because the makefile does not contain it. However, I find CPPFLAGS, which should stands for c++ flags, in makefile and then I use it to pass the desired option.
3.  Same as (I).(12).

5.  Building Qwt and QwtPlot3D in UB

1.  Change to `AlphaPlot*/3rdparty/qwt` and open qwtconfig.pri with text editor.
2.  Adding the block
`macx {  
 CONFIG += x86 ppc  
 }` 

4.  Same as (I).(18)--(20)
5.  Change to `AlphaPlot*/3rdparty/qwtplot3d` and open qwtplot3d.pro with text editor.
6.  Adding the block  
     `macx {  
     CONFIG += x86 ppc  
     }` after "QT += opengl". In fact, this location is somewhat arbitrary and not necessarily here.
7.  Same as (I).(22)--(24)

7.  Building AlphaPlot in UB

1.  Change to `AlphaPlot*/AlphaPlot` and open AlphaPlot.pro with text editor.
2.  Adding the block  
     `macx {  
     CONFIG += x86 ppc  
     }` in the section for "Default settings for Linux / Mac OS X"
3.  Same as (I).(25)--(35)

Closing remark: The described procedures are testes and built on OS X 10.5.4 and
a macbook. The older OS such as 10.4.x and PPC machine might need to minor
revision on the procedures. For example,
`QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk` might need to be added into the
\*.pro, `-isysroot /Developer/SDKs/MacOSX10.4u.sdk` need to be added into CFLAGS
and LDFLAGS.  

Good luck building AlphaPlot on Mac OS X. - Yu-Hung Lien  

## Additional information

### Forums and mailing lists:

If you want to discuss AlphaPlot with other users (or the developers) or if you are insterested in its further development, you can go to the [AlphaPlot forum](http://sourceforge.net/forum/?group_id=199120) or subscribe to the [AlphaPlot mailing list](http://sourceforge.net/mail/?group_id=199120).

### License:

AlphaPlot is distributed under the terms of the [GNU General Public License](http://www.gnu.org/licenses/gpl.html) as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. A copy of this license is provided in the file `gpl.txt`.

Thus it is ["free software"](http://www.fsf.org/licensing/essays/free-sw.html). "Free software" is a matter of liberty, not price. To understand the concept, you should think of "free" as in "free speech", not as in "free beer". "Free software" is also often called [Open Source, FOSS, or FLOSS](http://en.wikipedia.org/wiki/Alternative_terms_for_free_software). When we say that AlphaPlot is "free", we are talking about

*   The freedom to run the program, for any purpose (freedom 0).
*   The freedom to study how the program works, and adapt it to your needs (freedom 1). Access to the source code is a precondition for this.
*   The freedom to redistribute copies so you can help your neighbor (freedom 2).
*   The freedom to improve the program, and release your improvements to the public, so that the whole community benefits (freedom 3). Access to the source code is a precondition for this.

That said, AlphaPlot is also free as in "free beer". We do not charge money for anything you can download on our homepage and we will not do so in the future. See [here](http://AlphaPlot.sourceforge.net/contributing/index.html) for why we can do this and why we're working on AlphaPlot.  

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
