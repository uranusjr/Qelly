[![Build Status](https://travis-ci.org/uranusjr/Qelly.png?branch=master)](https://travis-ci.org/uranusjr/Qelly)

# Qelly

Nally, every platform, every machine.


## Introduction

Qelly is a Qt port of Nally, the open-source Telnet/SSH client for Mac OS X.
Qt is chosen to be the underlying framework of this application bacause it
enables us to build a GUI-based application with a native interface for every
operating system with minimal effort. The project is currently only a Qt version
of Nally (hence the name), but more features from other Telnet/SSH clients are
also planned.


## Licensing

Qelly is published under the terms of GNU Public License version 3 (GPL v3) as
defined by the Free Software Foundation (FSF). A copy of the license should
have been included with this file. You may also find the details of GPL v3 at:
http://www.gnu.org/licenses/gpl-3.0.txt

If you have any questions regarding the use of the content of the project,
feel free to contact the author(s) of the project. See AUTHORS for more
contact information.


## Special Notices on SSH Feature

The SSH connection is driven by external executables. In Unix-like systems, the
OpenSSH client, which is built-in in most cases, is used. Under Windows, Plink
(PuTTY Link command-line interface) is used.

You can get Plink at the PuTTY project download page:
http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html

Or, if you don't know which one to download, you can use this direct link:
http://the.earth.li/~sgtatham/putty/latest/x86/plink.exe

The SSH client path can be put anywhere you wish to, as long as you specify its
location correctly in Qelly's Preferences.


## Building

Qelly depends on Qt and LibQxt. Currently both Qt 4.8 and 5+ are supported. You
can find more information regarding installation on Qt and libqxt's respective
project pages:

* Qt 4.8: http://qt-project.org/doc/qt-4.8/installation.html
* Qt 5.1: http://qt-project.org/doc/qt-5.1/qtdoc/qtinstaller.html
* LibQxt: http://dev.libqxt.org/libqxt/wiki/user_guide

### APT-based Installation

If you use an APT-based package manager, the content of `.travis.yml` provides
a minimalistic build environment setup. It is currently based on Qt 4.8.1 and
LibQxt 0.6.1.

### Other Package Managers

Alternativelyf your package manager of choice (such as YUM, Homebrew, etc.)
provides Qt and LibQxt, you can install both them manually.

### LibQxt Setup Without Package Managers

If you wish to build LibQxt manually, you will need to choose an appropriate
version based on the version of Qt tha you use. At the time of writing
(2013-09-25), the newest LibQxt release (0.6.2) supports only Qt 4. Therefore,
if you wish to build Qelly against Qt 5, you will need to use Git or Mercurial
to clone the tip version, and build LibQxt from that.


## Thrid Party Components

Qelly uses Qt under the terms of GPL v3. The Qt GUI Toolkit (Qt) is a product of
Digia Plc and/or its subsidiary(-ies). Digia Plc, Qt, and their respective logos
are trademarks of Digia Plc. Visit Qt's project website for more information:
http://qt-project.org/

LibQxt is used under the terms of LGPL v2.1. LibQxt is owned by the Qxt
Foundation. http://libqxt.org/

Plink (PuTTY Link) is distributed under the MIT license. For more, see:
http://www.chiark.greenend.org.uk/~sgtatham/putty/licence.html

