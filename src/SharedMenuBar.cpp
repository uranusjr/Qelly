#include "SharedMenuBar.h"
#include <QKeySequence>
#include <QMenu>
#include "Globals.h"

namespace UJ
{

namespace Qelly
{

SharedMenuBar::SharedMenuBar(QWidget *parent) : QMenuBar(parent)
{
    QMenu *menu;
    connect(this, SIGNAL(editPreferences()), this, SIGNAL(preferences()));
    connect(this, SIGNAL(windowAbout()), this, SIGNAL(about()));

    menu = addMenu(tr("File"));
    menu->addAction(tr("New Tab"), this, SIGNAL(fileNewTab()),
                    QKeySequence(UJ::MOD | Qt::Key_T));
    menu->addAction(tr("Open Location…"), this, SIGNAL(fileOpenLocation()),
                    QKeySequence(UJ::MOD | Qt::Key_L));
    _reconnectAction = menu->addAction(tr("Reconnect"),
                                       this, SIGNAL(fileReconnect()));
    menu->addSeparator();
    menu->addAction(tr("Close Window"), this, SIGNAL(fileCloseWindow()),
                    QKeySequence(UJ::MOD | Qt::SHIFT | Qt::Key_W));
    menu->addAction(tr("Close Tab"), this, SIGNAL(fileCloseTab()),
                    QKeySequence(UJ::MOD | Qt::Key_W));

    menu = addMenu(tr("Edit"));
    menu->addAction(tr("Copy"), this, SIGNAL(editCopy()),
                    QKeySequence(UJ::MOD | Qt::Key_C));
    menu->addAction(tr("Paste"), this, SIGNAL(editPaste()),
                    QKeySequence(UJ::MOD | Qt::Key_P));
    menu->addAction(tr("Paste Wrap"), this, SIGNAL(editPasteWrap()),
                    QKeySequence(UJ::MOD | Qt::SHIFT | Qt::Key_V));
#ifdef Q_WS_MAC
    menu->addAction(tr("Paste Color"), this, SIGNAL(editPasteColor()),
                    QKeySequence(UJ::MOD | UJ::OPT | Qt::Key_V));
#else
    menu->addAction(tr("Paste Color"), this, SIGNAL(editPasteColor()),
                    QKeySequence(Qt::Key_AltGr, Qt::Key_V));
#endif
    menu->addAction(tr("Select All"), this, SIGNAL(editSelectAll()),
                    QKeySequence(UJ::MOD | Qt::Key_A));
    menu->addSeparator();
    menu->addAction(tr("Emicons…"), this, SIGNAL(editEmicons()),
                    QKeySequence(UJ::MOD | Qt::Key_E));
    menu->addSeparator();
#ifdef Q_WS_MAC
    menu->addAction(tr("Customize Toolbar…"),
                    this, SIGNAL(editCustomizeToolbar()));
#endif
    menu->addAction(tr("Preferences…"), this, SIGNAL(editPreferences()),
                    QKeySequence(UJ::MOD | Qt::Key_Comma));

    menu = addMenu(tr("View"));
    menu->addAction(tr("Anti-Idle"), this, SIGNAL(viewAntiIdle()));
    menu->addAction(tr("Show Hidden Text"), this, SIGNAL(viewShowHiddenText()));
    menu->addAction(tr("Detect Double Byte"),
                    this, SIGNAL(viewDetectDoubleByte()));
    menu->addSeparator();
    QMenu *encoding = menu->addMenu(tr("Encoding"));
    encoding->addAction(tr("Big5"), this, SIGNAL(viewEncodingBig5()));
    encoding->addAction(tr("GBK"), this, SIGNAL(viewEncodingGbk()));

    menu = addMenu(tr("Sites"));
    menu->addAction(tr("Edit Sites…"), this, SIGNAL(sitesEditSites()),
                    QKeySequence(UJ::MOD | Qt::Key_B));
    menu->addAction(tr("Add This Site"), this, SIGNAL(siteAddThisSite()),
                    QKeySequence(UJ::MOD | Qt::Key_D));

    menu = addMenu(tr("Window"));
    menu->addAction(new QAction(tr("About"), this));
#ifndef Q_WS_MAC
    menu->addSeperator();
#endif
    menu->addAction(tr("Minimize"), this, SIGNAL(windowMinimize()),
                    QKeySequence(UJ::MOD | Qt::Key_M));
    menu->addAction(tr("Zoom"), this, SIGNAL(windowZoom()));
    menu->addAction(tr("Select Next Tab"), this, SIGNAL(windowSelectNextTab()),
                    QKeySequence(UJ::MOD | Qt::Key_Right));
    menu->addAction(tr("Select Previous Tab"), this, SIGNAL(windowSelectPreviousTab()),
                    QKeySequence(UJ::MOD | Qt::Key_Left));
#ifdef Q_WS_MAC
    menu->addAction(tr("Bring All to Front"),
                    this, SIGNAL(windowBringAllToFront()),
                    QKeySequence(UJ::MOD | UJ::OPT | Qt::Key_Up));
#endif
    menu = addMenu(tr("Help"));
    menu->addAction(tr("Visit Project Home…"),
                    this, SIGNAL(helpVisitProjectHome()));
}

}   // namespace Qelly

}   // namespace UJ
