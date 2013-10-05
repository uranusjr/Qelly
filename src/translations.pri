# translations.pri
#
# Created: 04/10 2011 by uranusjr
#
# Copyright 2011 uranusjr. All rights reserved.
#
# This file may be distributed under the terms of GNU Public License version
# 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
# license should have been included with this file, or the project in which
# this file belongs to. You may also find the details of GPL v3 at:
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# If you have any questions regarding the use of this file, feel free to
# contact the author of this file, or the owner of the project in which
# this file belongs to.

# Get the lrealese binary's path
isEmpty(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

# Build a custom tool
# http://www.qtcentre.org/wiki/index.php?title=Undocumented_qmake#Custom_tools
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
# This following one is the main building command. -qm outputs the qm file to
# a specified path
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm \$(DESTDIR)/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link

# Use the custom tool and specify that make_all needs to make this one
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all
