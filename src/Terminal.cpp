#include "Terminal.h"
#include <cstring>
#include <QSet>
#include "AbstractConnection.h"
#include "Site.h"

namespace UJ
{

static unsigned short gEmptyAttr;

Terminal::Terminal(QObject *parent) : QObject(parent)
{
    initSettings();
    initCells();
    _csArg = new QQueue<int>();
    _csBuf = new QQueue<int>();
    _csTemp = 0;
}

Terminal::~Terminal()
{
    delete _csArg;
    delete _csBuf;
    delete [] _dirty;
    for (int i = 0; i < _row; i++)
        delete [] _cells[i];
    delete [] _cells;
}

void Terminal::initSettings()
{
    _savedCursorX = -1;
    _savedCursorY = -1;
    _row = 24;              // NOTE: To put in shared preferences
    _column = 80;           // NOTE: To put in shared preferences
    _scrollBeginRow = 0;
    _scrollEndRow = _row -1;
    _screenReverse = false;
    _originRelative = false;
    _autowrap = true;
    _lnm = true;
    _irm = false;
    _state = StateNormal;
    _standard = StandardVT102;
}

void Terminal::initCells()
{
    _cells = new BBS::Cell *[_row];
    for (int i = 0; i < _row; i++)
    {
        // In case _cursorX will exceed _column size (at the border of the
        // screen), we allocate one more unit for this array
        _cells[i] = new BBS::Cell[_column + 1];
    }
    _dirty = new int[_row * _column];
    clearAll();
}

void Terminal::startConnection()
{
    clearAll();
    // NOTE: Tell view to update
}

void Terminal::closeConnection()
{
    // NOTE: Tell view to update
}

void Terminal::clearAll()
{
    _cursorX = 0;
    _cursorY = 0;
    BBS::CellAttribute a;
    a.f.fColorIndex = 7;    // NOTE: To put in shared preferences
    a.f.bColorIndex = 0;    // NOTE: To put in shared preferences
    a.f.bright = 0;
    a.f.underlined = 0;
    a.f.blinking = 0;
    a.f.reversed = 0;
    a.f.isUrl = 0;
    a.f.isNothing = 0;
    gEmptyAttr = a.v;
    for (int i = 0; i < _row; i++)
        clearRow(i);

    _csBuf->clear();
    _csArg->clear();

    _fColorIndex = a.f.fColorIndex;
    _bColorIndex = a.f.bColorIndex;
    _csTemp = 0;
    _state = StateNormal;
    _bright = false;
    _underlined = false;
    _blinking = false;
    _reversed = false ^ _screenReverse;
}

void Terminal::clearRow(int row, int columnStart, int columnEnd)
{
    if (columnEnd == PositionNotFound)
        columnEnd = _column - 1;
    for (int x = columnStart; x <= columnEnd; x++)
    {
        _cells[row][x].byte = '\0';
        _cells[row][x].attr.v = gEmptyAttr;
        _cells[row][x].attr.f.bColorIndex = _bColorIndex;
        _cells[row][x].attr.f.reversed = _reversed;
        _dirty[row * _column + x] = true;
    }
}

void Terminal::reverseAll()
{
    for (int y = 0; y < _row; y++)
    {
        for (int x = 0; x < _column - 1; x++)
        {
            int colorIndex = _cells[y][x].attr.f.bColorIndex;
            _cells[y][x].attr.f.bColorIndex = _cells[y][x].attr.f.fColorIndex;
            _cells[y][x].attr.f.fColorIndex = colorIndex;
            _dirty[y * _column + x] = true;
        }
    }
}

void Terminal::setByteUnderCursor(uchar c)
{
    if (_cursorX <= _column - 1 && _irm)
    {
        for (int x = _column - 1; x > _cursorX; x--)
        {
            _cells[_cursorY][x] = _cells[_cursorY][x - 1];
            setDirtyAt(_cursorY, x);
        }
    }
    else if (_cursorX == _column && _autowrap)
    {
        _cursorX = 0;
        _hasWrapped = true;
        goOneRowDown();
    }
    _cells[_cursorY][_cursorX].byte = c;
    _cells[_cursorY][_cursorX].attr.f.fColorIndex = _fColorIndex;
    _cells[_cursorY][_cursorX].attr.f.bColorIndex = _bColorIndex;
    _cells[_cursorY][_cursorX].attr.f.bright = _bright;
    _cells[_cursorY][_cursorX].attr.f.underlined = _underlined;
    _cells[_cursorY][_cursorX].attr.f.blinking = _blinking;
    _cells[_cursorY][_cursorX].attr.f.reversed = _reversed;
    _cells[_cursorY][_cursorX].attr.f.isUrl = false;
    setDirtyAt(_cursorY, _cursorX);
    _cursorX++;
}

void Terminal::updateDoubleByteStateForRow(int row)
{
    BBS::Cell *cells = _cells[row];
    int db = 0;
    for (int i = 0; i < _column; i++)
    {
        switch (db)
        {
        case 0:
        case 2:
            if (cells[i].byte > 0x7f)
                db = 1;
            else
                db = 0;
            break;
        case 1:
            db = 2;
            break;
        }
        cells[i].attr.f.doubleByte = db;
    }
}

void Terminal::updateUrlStateForRow(int row)
{
    QSet<const char *> protocols;   // NOTE: Use global preferences
    protocols << "http://";
    protocols << "https://";
    protocols << "ftp://";
    protocols << "telnet://";
    protocols << "bbs://";
    protocols << "ssh://";
    protocols << "mailto:";

    bool isUrl = false;
    if (row > 0)
        isUrl = _cells[row - 1][_column - 1].attr.f.isUrl;

    BBS::Cell *cells = _cells[row];
    for (int i = 0; i < _column; i++)
    {
        if (isUrl)
        {
            uchar c = cells[i].byte;
            if (c < 0x21 || c > 0x7e || c == ')')
                isUrl = false;
        }
        else
        {
            foreach(const char *p, protocols)
            {
                bool matched = true;
                for (uint s = 0; s < ::strlen(p); s++)
                {
                    if (cells[i + s].byte != p[s])
                    {
                        matched = false;
                        break;
                    }
                }
                if (matched)
                {
                    isUrl = true;
                    break;
                }
            }
        }
        if (cells[i].attr.f.isUrl != isUrl)
        {
            cells[i].attr.f.isUrl = isUrl;
            setDirtyAt(row, i);
        }
    }
}

void Terminal::goOneRowDown(bool updateView)
{
    if (_cursorY == _scrollEndRow)
    {
        if (updateView)
            ;   // NOTE: Tell View to update and extend bottom
        BBS::Cell *emptyLine = _cells[_scrollBeginRow];
        clearRow(_scrollBeginRow);
        for (int x = _scrollBeginRow; x < _scrollEndRow; x++)
            _cells[x] = _cells[x + 1];
        _cells[_scrollEndRow] = emptyLine;
        setDirtyAll();
    }
    else
    {
        _cursorY++;
        if (_cursorY >= _scrollEndRow)
            _cursorY = _row - 1;
    }
}

void Terminal::goOneRowUp(bool updateView)
{
    if (_cursorY == _scrollBeginRow)
    {
        if (updateView)
            ;   // NOTE: Tell view to update & extend top
        BBS::Cell *emptyLine = _cells[_scrollEndRow];
        clearRow(_scrollEndRow);
        for (int x = _scrollEndRow; x > _scrollBeginRow; x--)
            _cells[x] = _cells[x - 1];
        _cells[_scrollBeginRow] = emptyLine;
        setDirtyAll();
    }
    else
    {
        _cursorY--;
        if (_cursorY < 0)
            _cursorY = 0;
    }
}

void Terminal::feedData(QByteArray &data)
{
    for (int i = 0; i < data.size(); i++)
    {
        uchar c = static_cast<uchar>(data[i]);
        switch (_state)
        {
        case StateNormal:
            handleNormalDataInput(c);
            break;
        case StateEscape:
            handleEscapeDataInput(c, &i, data);
            break;
        case StateScs:
            _state = StateNormal;   // NOTE: NEED IMPLEMENTATION
            break;
        case StateControl:
            handleControlDataInput(c);
        }
    }

    for (int i = 0; i < _row; i++)
    {
        updateDoubleByteStateForRow(i);
        updateUrlStateForRow(i);
    }
    // NOTE: Tell view to tick()
}

void Terminal::handleNormalDataInput(uchar c)
{
    switch (c)
    {
    case ASC_NUL:   // NULL
    case ASC_ETX:   // Flow control
    case ASC_EQT:   // Flow control
        break;
    case ASC_ENQ:   // Flow control...why does this need action?
        uchar cmd[1];
        cmd[0] = ASC_NUL;
        connection()->sendBytes(cmd, 1);
        break;
    case ASC_ACK:   // Flow control
        break;
    case ASC_BEL:   // Bell
        // NOTE: Ring a bell
        setHasMessage();
        break;
    case ASC_BS:    // Backspace (^H)
        handleNormalBs();
        break;
    case ASC_HT:    // Horizontal Tab
        _cursorX = (_cursorX / 8 + 1) * 8;
        break;
    case ASC_LF:    // Line feed
    case ASC_VT:    // Vertical Tab
    case ASC_FF:    // Form feed
        if (!_lnm)
            _cursorX = 0;
        goOneRowDown(false);
        break;
    case ASC_CR:    // Carriage return
        _cursorX = 0;
        break;
    case ASC_LS1:   // Locked Shift 1
    case ASC_LS0:   // Locked Shift 0
        // These two sets terminal to designated character set
        // Ignore it for now...
        break;
    case ASC_DLE:   // Normally for modem
    case ASC_DC1:   // XON
    case ASC_DC2:
    case ASC_DC3:   // XOFF
    case ASC_DC4:
    case ASC_NAK:
    case ASC_SYN:
    case ASC_ETB:
        break;
    case ASC_CAN:   // Canonical
    case ASC_SUB:   // Substitute
        // These two can cancel the sequence and display SUB character.
        // Ignore for now.
        break;
    case ASC_EM:    // ^Y
        break;
    case ASC_ESC:   // Escape
        _state = StateEscape;
        break;
    case ASC_FS:
    case ASC_GS:
    case ASC_RS:
    case ASC_US:
    case ASC_DEL:   // Delete; ignored on input
        break;
    default:
        setByteUnderCursor(c);
        break;
    }
}

void Terminal::handleNormalBs()
{
    if (_cursorX > 0)
    {
        if (_cursorX == _column && _autowrap)
            _cursorX--;     // vt console bug/feature
        _cursorX--;
    }
    else if (_cursorX == 0 && _hasWrapped)  // Reverse wrap
    {
        _cursorX = _column - 1;
        goOneRowUp();
        _hasWrapped = false;
    }
}

void Terminal::handleEscapeDataInput(uchar c, int *p_i, QByteArray &data)
{
    switch (c)
    {
    case ASC_ESC:   // Double Esc
        _state = StateEscape;
        break;
    case ESC_CSI:
        _csArg->clear();
        _csBuf->clear();
        _csTemp = 0;
        _state = StateControl;
        break;
    case ESC_RI:    // Scroll down (cursor up)
        goOneRowUp();
        _state = StateNormal;
        break;
    case ESC_IND:   // Index, scroll up (cursor down)
        goOneRowDown();
        _state = StateNormal;
        break;
    case ESC_DECSC: // Save cursor
        _savedCursorX = _cursorX;
        _savedCursorY = _cursorY;
        _state = StateNormal;
        break;
    case ESC_DECRC: // Restore cursor
        _cursorX = _savedCursorX;
        _cursorY = _savedCursorY;
        break;
    case ESC_HASH:
        handleEscapeHash(p_i, data);
        _state = StateNormal;
        break;
    case ESC_sG0:   // Font sets
    case ESC_sG1:
        _state = StateScs;
        break;
    case ESC_APPK:  // Application keyboard mode (vt52)
    case ESC_NUMK:  // Numeric keyboard mode (vt52)
        _state = StateNormal;
        break;
    case ESC_NEL:   // Next line (CR + Index)
        _cursorX = 0;
        goOneRowDown();
        _state = StateNormal;
        break;
    case ESC_HTS:   // NOTE: NEED IMPLEMENTATION
        _state = StateNormal;
        break;
    case ESC_RIS:   // RIS reset
        clearAll();
        _cursorX = 0;
        _cursorY = 0;
        _state = StateNormal;
        break;
    default:
        _state = StateNormal;
        break;
    }
}

void Terminal::handleEscapeHash(int *p_i, QByteArray &data)
{
    int i = *(p_i);
    if (i < data.size() - 1 && data[i + 1] == '8')  // DECALN (fill with E)
    {
        *p_i = i + 1;
        for (int y = 0; y < _row - 1; y++)
        {
            for (int x = 0; x <= _column - 1; x++)
            {
                _cells[y][x].byte = 'E';
                _cells[y][x].attr.v = gEmptyAttr;
                _dirty[y * _column + x] = true;
            }
        }
    }
}

void Terminal::handleControlDataInput(uchar c)
{
    if (c % 0x10 == 3)
    {
        _csBuf->enqueue(static_cast<int>(c));
        if (c >= '0' && c <= '9')
        {
            _csTemp = _csTemp * 10 + (c - '0');
        }
        else if (c == '?')
        {
            _csArg->enqueue(-1);
            _csTemp = 0;
            _csBuf->clear();
        }
        else if (!_csBuf->isEmpty())
        {
            _csArg->enqueue(_csTemp);
            _csTemp = 0;
            _csBuf->clear();
        }
    }
    else
    {
        switch (c)
        {
        case ASC_BS:    // Backspace
            if (!_csBuf->isEmpty())
                _csArg->dequeue();
            break;
        case ASC_VT:    // Vertical tab
            if (!_lnm)
                _cursorX = 0;
            goOneRowDown(false);
            break;
        case ASC_CR:    // Carriage return
            _cursorX = 0;
            break;
        default:
            handleControlNonSimpleShiftingInputs(c);
            break;
        }
    }
    _csArg->clear();
    _state = StateNormal;
}

void Terminal::handleControlNonSimpleShiftingInputs(uchar c)
{
    if (!_csBuf->isEmpty())
    {
        _csArg->enqueue(_csTemp);
        _csTemp = 0;
        _csBuf->clear();
    }
    int p;
    switch (c)
    {
    case CSI_ICH:
        p = popLineCount();
        for (int x = _column - 1; x > _cursorX + p - 1; x--)
        {
            _cells[_cursorY][x] = _cells[_cursorY][x - p];
            setDirtyAt(_cursorY, x);
        }
        clearRow(_cursorY, _cursorX, _cursorX + p - 1);
        break;
    case CSI_CUU:
        _cursorY -= popLineCount();
        if (_originRelative && _cursorY < _scrollBeginRow)
            _cursorY = _scrollBeginRow;
        else if (_cursorY < 0)
            _cursorY = 0;
        break;
    case CSI_CUD:
        _cursorY += popLineCount();
        if (_originRelative && _cursorY > _scrollEndRow)
            _cursorY = _scrollEndRow;
        else if (_cursorY >= _row)
            _cursorY = _row - 1;
        break;
    case CSI_CUF:
        _cursorX += popLineCount();
        if (_cursorX >= _column)
            _cursorX = _column - 1;
        break;
    case CSI_CUB:
        _cursorX -= popLineCount();
        if (_cursorX < 0)
            _cursorX = 0;
        break;
    case CSI_CNL:
        _cursorX = 0;
        _cursorY += popLineCount();
        if (_cursorY >= _row)
            _cursorY = _row - 1;
        break;
    case CSI_CPL:
        _cursorX = 0;
        _cursorY -= popLineCount();
        if (_cursorY < 0)
            _cursorY = 0;
        break;
    case CSI_CHA:
        p = _csArg->isEmpty() ? 0 : popLineCount() - 1;
        moveCursorTo(p, _cursorY);
        break;
    case CSI_HVP:   // Cursor position
    case CSI_CUP:   // ^[{y};{x}H   Go to row {y}, column {x} (default 1)
        handleControlCup();
        break;
    case CSI_ED:            // Erase page (cursor does not move)
        handleControlEd();  // ^[{n}J   {n} = 0 from cursor to end (default)
        break;              //                1 from start to cursor
                            //                2 erase whole page
    case CSI_EL:    // Erase line (similar to CSI_ED)
        handleControlEl();
        break;
    case CSI_IL:    // Insert line
        handleControlIl();
        break;
    case CSI_DL:    // Delete line
        handleControlDl();
        break;
    case CSI_DCH:   // Delete character under cursor
        handleControlDch();
        break;
    case CSI_HPA:   // Go to horizontal absolute position
        p = popLineCount() - 1;
        moveCursorTo(p, _cursorY);
        break;
    case CSI_HPR:   // Go to horizontal relative position
        p = popLineCount();
        moveCursorTo(_cursorX + p, _cursorY);
        break;
    case CSI_REP:   // Repeat
        break;      // NOTE: NEED IMPLEMENTATION
    case CSI_DA:    // Identity request
        handleControlDa();
        break;
    case CSI_VPA:   // Go to vertical absolute position
        p = popLineCount() - 1;
        moveCursorTo(_cursorX, p);
        break;
    case CSI_VPR:   // Go to vertical relative position
        p = popLineCount();
        moveCursorTo(_cursorX, _cursorY + p);
        break;
    case CSI_TBC:   // Clear tab at current column
        break;      // NOTE: NEED IMPLEMENTATION
    case CSI_SM:    // Set mode
        handleControlSm();
        break;
    case CSI_HPB:   // Move to horizontal relative (back) position
        p = popLineCount();
        moveCursorTo(_cursorX - p, _cursorY);
        break;
    case CSI_VPB:   // Move to vertical relative (back) position
        p = popLineCount();
        moveCursorTo(_cursorX, _cursorY - p);
        break;
    case CSI_RM:    // Reset mode
        handleControlRm();
        break;
    case CSI_SGR:   // Character attributes
        handleControlSgr();
        break;
    case CSI_DSR:   // Description request
        handleControlDsr();
        break;
    case CSI_DECSTBM:   // Scrolling region assignment
        handleControlDecstbm();
        break;
    case CSI_SCP:
        _savedCursorX = _cursorX;
        _savedCursorY = _cursorY;
        break;
    case CSI_RCP:
        if (_savedCursorX >= 0 && _savedCursorY >= 0)
        {
            _cursorX = _savedCursorX;
            _cursorY = _savedCursorY;
        }
        break;
    default:
        break;
    }
}

void Terminal::handleControlCup()
{
    if (_csArg->isEmpty())
    {
        _cursorX = 0;
        _cursorY = 0;
    }
    else
    {
        int p = popLineCount();
        int q = popLineCount();
        if (_originRelative && _scrollBeginRow > 0)
        {
            p += _scrollBeginRow;
            if (p > _scrollEndRow)
                p = _scrollEndRow + 1;
        }
        moveCursorTo(q - 1, p - 1);
    }
}

void Terminal::handleControlEd()
{
    if (_csArg->isEmpty() || _csArg->head() == 0)
    {
        clearRow(_cursorY, _cursorX, _column - 1);
        for (int y = _cursorY + 1; y < _row; y++)
            clearRow(y);
    }
    else
    {
        switch (_csArg->head())
        {
        case 1:
            clearRow(_cursorY, 0, _cursorX);
            for (int y = 0; y < _cursorY; y++)
                clearRow(y);
            break;
        case 2:
            clearAll();
            break;
        default:
            break;
        }
    }
}

void Terminal::handleControlEl()
{
    if (_csArg->isEmpty() || _csArg->head() == 0)
    {
        clearRow(_cursorY, _cursorX, _column - 1);
    }
    else
    {
        switch (_csArg->head())
        {
        case 1:
            clearRow(_cursorY, 0, _cursorX);
            break;
        case 2:
            clearRow(_cursorY);
            break;
        default:
            break;
        }
    }
}

void Terminal::handleControlIl()
{
    int lineNum = popLineCount();
    for (int l = 0; l < lineNum; l++)
    {
        clearRow(_scrollEndRow);
        BBS::Cell *emptyLine = cellsAtRow(_scrollEndRow);
        for (int y = _scrollEndRow; y > _cursorY; y--)
            _cells[y] = _cells[y - 1];
        _cells[_cursorY] = emptyLine;
    }
    for (int l = _cursorY; l <= _scrollEndRow; l++)
        setDirtyUnder(l);
}

void Terminal::handleControlDl()
{
    int lineNum = popLineCount();
    for (int l = 0; l < lineNum; l++)
    {
        clearRow(_cursorY);
        BBS::Cell *emptyLine = cellsAtRow(_cursorY);
        for (int y = _cursorY; y < _scrollEndRow; y++)
            _cells[y] = _cells[y + 1];
        _cells[_scrollEndRow] = emptyLine;
    }
    for (int l = _cursorY; l <= _scrollEndRow; l++)
        setDirtyUnder(l);
}

void Terminal::handleControlDch()
{
    int p = popLineCount();
    for (int x = _cursorX; x <= _column - 1; x++)
    {
        if (x <= _column - 1 - p)
        {
            _cells[_cursorY][x] = _cells[_cursorY][x + p];
        }
        else
        {
            _cells[_cursorY][x].byte = '\0';
            _cells[_cursorY][x].attr.v = gEmptyAttr;
            _cells[_cursorY][x].attr.f.bColorIndex = _bColorIndex;
        }
        setDirtyAt(_cursorY, x);
    }
}

void Terminal::handleControlDa()
{
    uchar cmd[10];  // Enough for now
    uint cmd_sz = 0;
    switch (_standard)
    {
    case StandardVT100: // respond ESC[?1;0c
        cmd[cmd_sz++] = '\x1b'; // ESC
        cmd[cmd_sz++] = '[';
        cmd[cmd_sz++] = '?';
        cmd[cmd_sz++] = '1';
        cmd[cmd_sz++] = ';';
        cmd[cmd_sz++] = '0';
        cmd[cmd_sz++] = 'c';
        break;
    case StandardVT102: // respond ESC[?6c
        cmd[cmd_sz++] = '\x1b'; // ESC
        cmd[cmd_sz++] = '[';
        cmd[cmd_sz++] = '?';
        cmd[cmd_sz++] = '0';
        cmd[cmd_sz++] = 'c';
        break;
    default:
        break;
    }
    if (_csArg->isEmpty())
        connection()->sendBytes(cmd, cmd_sz);
    else if (_csArg->size() == 1 && _csArg->head() == 0)
        connection()->sendBytes(cmd, cmd_sz);
}

void Terminal::handleControlSm()
{
    bool clear = false;
    while (!_csArg->isEmpty())
    {
        int p = _csArg->dequeue();
        switch (p)
        {
        case 20:    // Set mew line mode
            _lnm = false;
            break;
        case 4:     // Set to INSERT mode
            _irm = true;
            break;
        case 1:     // Cursor key send ESC 0 prefix instead of ESC [
        case 2:     // Keyboard action mode
        case 6:     // Erasure
        case 12:    // SRM
            break;
        case -1:
            if (_csArg->size() == 1)
            {
                p = _csArg->dequeue();
                switch (p)
                {
                case 3:     // Number of columns <- 132
                    clear = true;
                    _originRelative = false;
                    _scrollBeginRow = 0;
                    _scrollEndRow = _row - 1;
                    break;
                case 5:     // Reverse
                    if (!_screenReverse)
                    {
                        _screenReverse = true;
                        _reversed = !_reversed;
                        reverseAll();
                    }
                    break;
                case 6:     // Relative origin
                    _originRelative = true;
                    break;
                case 7:     // Auto-wrap
                    _autowrap = true;
                case 1:     // Set cursor key to application
                case 4:     // Smooth scrolling
                case 8:     // Auto-repeating
                case 9:     // Interlacing
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    if (clear)
    {
        if (!_originRelative)
        {
            clearAll();
            _cursorX = 0;
            _cursorY = 0;
        }
    }
}

void Terminal::handleControlRm()
{
    bool clear = false;
    while (!_csArg->isEmpty())
    {
        int p = _csArg->dequeue();
        switch (p)
        {
        case 20:    // Set mew line mode
            _lnm = true;
            break;
        case 4:     // Set to INSERT mode
            _irm = false;
            break;
        case 1:     // Cursor key send ESC 0 prefix instead of ESC [
        case 2:     // Keyboard action mode
        case 6:     // Erasure
        case 12:    // SRM
            break;
        case -1:
            if (_csArg->size() == 1)
            {
                p = _csArg->dequeue();
                switch (p)
                {
                case 3:     // Number of columns reset is not supported
                    clear = true;
                    _originRelative = false;
                    _scrollBeginRow = 0;
                    _scrollEndRow = _row - 1;
                    break;
                case 5:     // Reverse
                    if (!_screenReverse)
                    {
                        _screenReverse = false;
                        _reversed = !_reversed;
                        reverseAll();
                    }
                    break;
                case 6:     // Relative origin
                    _originRelative = false;
                    break;
                case 7:     // Auto-wrap
                    _autowrap = false;
                case 1:     // Set cursor key to application
                case 4:     // Smooth scrolling
                case 8:     // Auto-repeating
                case 9:     // Interlacing
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
    if (clear)
    {
        clearAll();
        _cursorX = 0;
        _cursorY = 0;
    }
}

void Terminal::handleControlSgr()
{
    if (_csArg->isEmpty())  // No parameters means clear, same as 0
        _csArg->enqueue(0);

    while (!_csArg->isEmpty())
    {
        int p = _csArg->dequeue();
        switch (p)
        {
        case 0:
            _fColorIndex = 7;
            _bColorIndex = 9;
            _bright = false;
            _underlined = false;
            _blinking = false;
            _reversed = false ^ _screenReverse;
            break;
        case 1:
            _bright = true;
            break;
        case 4:
            _underlined = true;
            break;
        case 5:
            _blinking = true;
            break;
        case 7:
            _reversed = true ^ _screenReverse;
            break;
        default:
            switch (p / 10)
            {
            case 3:     // p = 3x, foreground color
                _fColorIndex = p - 30;
                break;
            case 4:     // p = 4x, background color
                _bColorIndex = p - 40;
                break;
            default:
                break;
            }
            break;
        }
    }
}

void Terminal::handleControlDsr()
{
    if (_csArg->size() != 1)
        return;

    int p = _csArg->dequeue();
    uchar cmd[10];      // Longest possible cursor position msg is 8
    uint cmd_sz = 0;
    switch (p)
    {
    case 5:     // Report device OK ESC[0n
        cmd[cmd_sz++] = '\x1b';
        cmd[cmd_sz++] = '[';
        cmd[cmd_sz++] = '0';
        cmd[cmd_sz++] = 'n';
        connection()->sendBytes(cmd, cmd_sz);
        break;
    case 6:     // Report cursor ESC[{y};{x}R  ({x}, {y} indicate position)
        cmd[cmd_sz++] = '\x1b';
        cmd[cmd_sz++] = '[';
        if ((_cursorY + 1) >= 10)                   // 10s (may not exist)
            cmd[cmd_sz++] = '0' + (_cursorY + 1) / 10;
        cmd[cmd_sz++] = '0' + (_cursorY + 1) % 10;  // 1s
        if ((_cursorX + 1) >= 10)                   // 10s (may not exist)
            cmd[cmd_sz++] = '0' + (_cursorX + 1) / 10;
        cmd[cmd_sz++] = '0' + (_cursorX + 1) % 10;  // 1s
        cmd[cmd_sz++] = CSI_CPR;
        connection()->sendBytes(cmd, cmd_sz);
        break;
    default:
        break;
    }
}

void Terminal::handleControlDecstbm()
{
    if (_csArg->isEmpty())
    {
        _scrollBeginRow = 0;
        _scrollEndRow = _row - 1;
    }
    else if (_csArg->size() == 2)
    {
        int large = _csArg->dequeue();
        int small = _csArg->dequeue();
        if (small > large)
        {               // Swap
            int t = small;
            small = large;
            large = t;
        }
        _scrollBeginRow = small - 1;
        _scrollEndRow = large - 1;
    }
    _cursorX = 0;
    _cursorY = _scrollBeginRow;
}

QString Terminal::urlStringAt(int row, int column, bool *hasUrl)
{
    *hasUrl = _cells[row][column].attr.f.isUrl;
    if (!*hasUrl)
        return QString();

    // Find the head of the string
    while (_cells[row][column].attr.f.isUrl)
    {
        column--;
        if (column < 0)
        {
            column = _column - 1;
            row--;
        }
        if (row < 0)
            break;
    }

    // Set to the first character
    column++;
    if (column >= _column)
    {
        column = 0;
        row++;
    }

    QString url;
    while (_cells[row][column].attr.f.isUrl)
    {
        url.append(_cells[row][column].byte);
        column++;
        if (column >= _column)
        {
            column = 0;
            row++;
        }
        if (row >= _row)
            break;
    }
    return url;
}

BBS::Encoding Terminal::encoding() const
{
    return connection()->site()->encoding();
}

void Terminal::setEncoding(BBS::Encoding encoding)
{
    connection()->site()->setEncoding(encoding);
}

}   // namespace UJ
