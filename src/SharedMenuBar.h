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
    void fileCloseWindow();
    void fileCloseTab();
    void editCut();
    void editCopy();
    void editPaste();
    void editPasteWrap();
    void editPasteColor();
    void editSelectAll();
    void editEmicons();
    void editCustomizeToolbar();    // Mac only...
    void editPreferences();
    void viewAntiIdle();
    void viewShowHiddenText();
    void viewDetectDoubleByte();
    void viewEncodingBig5();
    void viewEncodingGbk();
    void sitesEditSites();
    void siteAddThisSite();
    void windowMinimize();
    void windowZoom();
    void windowSelectNextTab();
    void windowSelectPreviousTab();
    void windowBringAllToFront();
    void windowAbout();
    void helpVisitProjectHome();

private:
    QAction *_reconnectAction;

public:
    inline QAction *reconnectAction() const
    {
        return _reconnectAction;
    }

};

}   // namespace Qelly

}   // namespace UJ

#endif // SHAREDMENUBAR_H
