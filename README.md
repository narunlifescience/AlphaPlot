#![logo](data/icons/app/22/AlphaPlot.png)Alpha Plot

[![Build Status](https://travis-ci.org/narunlifescience/AlphaPlot.svg?branch=master)](https://travis-ci.org/narunlifescience/AlphaPlot) [![Join the chat at https://gitter.im/narunlifescience/AlphaPlot](https://badges.gitter.im/narunlifescience/AlphaPlot.svg)](https://gitter.im/narunlifescience/AlphaPlot?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Alpha Plot is a free application for <i>Sci</i>entific <i>D</i>ata <i>A</i>nalysis and <i>Vis</i>ualization for Windows, Linux and Mac OS X (probably BSD also).

# Installation
Get the code (if you haven't already):

    https://github.com/narunlifescience/AlphaPlot.git

Compile and install:

    qmake
    make 
    sudo make install

For Windows/OSX see [installation notes](data/INSTALL.md)


Opening an issue
----------------
### Ask for a new feature

Please:

 * Check if the new feature is not already implemented (Changelog)
 * Check if another person didn't already open an issue
 * If there is already an opened issue there is no need to comment "+1", it won't help. Instead, you can subscribe to the issue to be notified of anything new about it

### Report a bug

Please:
 
 * Try the latest developer build to see if the bug is still present (**Attention**, those builds aren't stable so they might not work well and could sometimes break things like user settings). If it works like a charm even though you see an open issue, please comment on it and explain that the issue has been fixed
 * Check if another person has already opened the same issue to avoid duplicates
 * If there already is an open issue you could comment on it to add precisions about the problem or confirm it
 * In case there isn't, you can open a new issue with an explicit title and as much information as possible (OS, Alpha Plot version, how to reproduce the problem...)
 * Please use http://pastebin.com/ for logs/debug
 
If there are no answers, it doesn't mean we don't care about your feature request/bug. It just means we can't reproduce the bug or haven't had time to implement it :o)

## Dependencies

The following packages may be required by Alpha plot ...

- Qt (https://download.qt.io/archive/qt/4.8/4.8.5/),
- Qwt (http://qwt.sourceforge.net/),
- Qwtplot3D (http://qwtplot3d.sourceforge.net/),
- muParser (http://muparser.beltoforion.de/),
- Python (http://www.python.org/),
- GSL (http://www.gnu.org/software/gsl/)
