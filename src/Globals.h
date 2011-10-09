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

namespace BBS
{

enum Encoding
{
    EncodingBig5,
    EncodingGBK
};

enum AnsiColorKey
{
    ColorKeyCtrlU,
    ColorKeyDoubleEsc
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
