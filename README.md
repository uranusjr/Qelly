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


## Thrid Party Components

Qelly uses Qt under the terms of GPL v3. The Qt GUI Toolkit (Qt) is a product of
Digia Plc and/or its subsidiary(-ies). Digia Plc, Qt, and their respective logos
are trademarks of Digia Plc. Visit Qt's project website for more information:
http://qt-project.org/

Plink (PuTTY Link) is distributed under the MIT license. For more, see:
http://www.chiark.greenend.org.uk/~sgtatham/putty/licence.html

