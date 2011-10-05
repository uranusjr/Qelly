#ifndef SHAREDMENUBAR_H
#define SHAREDMENUBAR_H

#include <QMenuBar>

namespace UJ
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
};

}   // namespace UJ

#endif // SHAREDMENUBAR_H
