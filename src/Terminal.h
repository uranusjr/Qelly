#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QQueue>
#include "UJCommonDefs.h"
#include "Globals.h"
#include "YLTerminal.h"

namespace UJ
{

namespace Connection
{
class AbstractConnection;
}

class Terminal : public QObject
{
    Q_OBJECT

public:
    explicit Terminal(QObject *parent = 0);
    virtual ~Terminal();
    bool isDiryAt(int row, int column);
    BBS::CellAttribute attributeOfCellAt(int row, int column);
    inline BBS::Cell *cellsAtRow(int row)
    {
        return _cells[row];
    }
    QString stringFromIndex(int begin, int length);
    QString urlStringAt(int row, int column, bool *haUrl);

signals:

public slots:
    void startConnection();
    void closeConnection();
    void feedData(QByteArray &data);
    void clearAll();
    void clearRow(int row,
                  int columnStart = 0, int columnEnd = PositionNotFound);
    inline void setDirtyAll()
    {
        setDirtyUnder(0);
    }
    inline void setDirtyUnder(int row, bool dirty = true)
    {
        for (int i = row * _column; i < _row * _column; i++)
            _dirty[i] = dirty;
    }
    inline void setDirtyAt(int row, int column, bool dirty = true)
    {
        _dirty[row * _column + column] = dirty;
    }
    void reverseAll();
    void updateUrlStateForRow(int row);
    void updateDoubleByteStateForRow(int row);

private:
    void initSettings();
    void initCells();
    void setByteUnderCursor(uchar c);
    inline void moveCursorTo(int x, int y)
    {
        _cursorX = x < 0 ? 0 : (_cursorX >= _column ? _column - 1 : x);
        _cursorY = y < 0 ? 0 : (_cursorY >= _row ? _row - 1 : y);
    }
    inline int popLineCount()
    {
        if (_csArg->isEmpty())
            return 1;
        int p = _csArg->dequeue();
        return (p > 1 ? p : 1);
    }
    void goOneRowUp(bool updateView = true);
    void goOneRowDown(bool updateView = true);
    void handleNormalDataInput(uchar c);
    void handleEscapeDataInput(uchar c, int *p_i, QByteArray &data);
    void handleControlDataInput(uchar c);
    void handleNormalBs();
    void handleEscapeHash(int *p_i, QByteArray &data);
    void handleControlNonSimpleShiftingInputs(uchar c);
    void handleControlCup();
    void handleControlEd();
    void handleControlEl();
    void handleControlIl();
    void handleControlDl();
    void handleControlDch();
    void handleControlDa();
    void handleControlSm();
    void handleControlRm();
    void handleControlSgr();
    void handleControlDsr();
    void handleControlDecstbm();

    // NOTE: The Telnet View Instance Here
    Connection::AbstractConnection *_connection;
    QQueue<int> *_csArg;
    QQueue<int> *_csBuf;
    uint _csTemp;

    int _row;
    int _column;
    int _cursorX;
    int _cursorY;
    int _savedCursorX;
    int _savedCursorY;
    int _scrollBeginRow;
    int _scrollEndRow;
    bool _hasMessage;
    bool _hasWrapped;

    int _fColorIndex;
    int _bColorIndex;
    bool _bright;
    bool _underlined;
    bool _blinking;
    bool _reversed;

    BBS::Cell **_cells;
    int *_dirty;

    bool _screenReverse;  // reverse (true), not reverse (false, default)
    bool _originRelative; // relative origin (true), absolute (false, default)
    bool _autowrap;       // autowrap (true, default), wrap disabled (false)
    bool _lnm;            // line feed (true, default), new line (false)
    bool _irm;            // insert (true), replace (false, default)

    enum State
    {
        StateNormal,
        StateEscape,
        StateControl,
        StateScs
    } _state;

    enum Standard
    {
        StandardVT100,
        StandardVT102
    } _standard;

public: // Setters & Getters
    inline int cursorRow() const
    {
        return _cursorY;
    }
    inline void setCursorRow(int row)
    {
        _cursorY = row;
    }
    inline int cursorColumn() const
    {
        return _cursorX;
    }
    inline void setCursorColumn(int column)
    {
        _cursorX = column;
    }
    BBS::Encoding encoding() const;
    void setEncoding(BBS::Encoding encoding);
    inline bool hasMessage() const
    {
        return _hasMessage;
    }
    inline void setHasMessage(bool hasMessage = true)
    {
        _hasMessage = hasMessage;
        // NOTE: Change Tab Icon...Maybe should be a signal connected to view
    }
    inline Connection::AbstractConnection *connection() const
    {
        return _connection;
    }
    inline void setConnection(Connection::AbstractConnection *connection)
    {
        _connection = connection;
    }
};

}   // namespace UJ

#endif // TERMINAL_H
