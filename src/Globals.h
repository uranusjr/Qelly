#ifndef GLOBALS_H
#define GLOBALS_H

#include <Qt>

namespace UJ
{

enum Modifiers
{
#ifdef Q_OS_MAC
    MOD = Qt::META,
    OPT = Qt::ALT
#else
    MOD = Qt::ALT,
    OPT = Qt::META
#endif
};

// Due to a bug in Qt, cmd and control keys are swapped on Macs even when we
// explicitly disable the swap. We need to swap it back.
enum Keys
{
#ifdef Q_OS_MAC
    Key_Mod = Qt::Key_Control,
    Key_Control = Qt::Key_Meta,
    Key_Meta = Qt::Key_Control
#else
    Key_Mod = Qt::Key_Alt,
    Key_Control = Qt::Key_Control,
    Key_Meta = Qt::Key_Meta
#endif
};

namespace Connection
{

enum Type
{
    TypeTelnet,
    TypeSsh,
    TypeUnknown
};

}   // namespace Connection

namespace BBS
{

enum Encoding
{
    EncodingUnknown,
    EncodingBig5,
    EncodingGBK
};

enum AnsiColorKey
{
    ColorKeyCtrlU,
    ColorKeyDoubleEsc
};

enum Size
{
    SizeRowCount = 24,
    SizeColumnCount = 80
};

enum ColorIndex
{
    ColorIndexBackground = 0,
    ColorIndexForeground = 7
};

union CellAttribute
{
    ushort v;
    struct
    {
        uint fColorIndex    : 4;
        uint bColorIndex    : 4;
        uint bright         : 1;
        uint underlined     : 1;
        uint blinking       : 1;
        uint reversed       : 1;
        uint doubleByte     : 2;
        uint isUrl          : 1;
        uint isNothing      : 1;
    } f;
};

struct Cell
{
    uchar byte;
    CellAttribute attr;
};

}   // namespace BBS

}   // namespace UJ

#endif // GLOBALS_H
