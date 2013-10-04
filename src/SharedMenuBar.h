/*****************************************************************************
 * SharedMenuBar.h
 *
 * Created: 05/10 2011 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

#ifndef SHAREDMENUBAR_H
#define SHAREDMENUBAR_H

#include <QMenuBar>

namespace UJ
{

namespace Qelly
{

class SharedMenuBar : public QMenuBar
{
    Q_OBJECT

public:
    explicit SharedMenuBar(QWidget *parent = 0);
    static inline SharedMenuBar *sharedInstance()
    {
        static SharedMenuBar *g = new SharedMenuBar();
        return g;
    }

signals:
    void about();
    void preferences();
    void fileNewTab();
    void fileOpenLocation();
    void fileReconnect();
    void fileQuit();
    void fileCloseTab();
    void editCopy();
    void editPaste();
    void editPasteWrap();
    void editPasteColor();
    void editSelectAll();
    void editEmoticons();
    void editCustomizeToolbar();    // Mac only...
    void editPreferences();
    void viewAntiIdle(bool);
    void viewShowHiddenText(bool);
    void sitesEditSites();
    void siteAddThisSite();
    void windowMinimize();
    void windowSelectNextTab();
    void windowSelectPreviousTab();
    void windowBringAllToFront();
    void windowAbout();
    void windowAcknowledgement();
    void helpVisitProjectHome();
};

}   // namespace Qelly

}   // namespace UJ

#endif // SHAREDMENUBAR_H
