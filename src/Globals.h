#ifndef GLOBALS_H
#define GLOBALS_H

namespace UJ
{

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
    unsigned short v;
    struct
    {
        unsigned int fColorIndex    : 4;
        unsigned int bColorIndex    : 4;
        unsigned int bright         : 1;
        unsigned int underlined     : 1;
        unsigned int blinking       : 1;
        unsigned int reversed       : 1;
        unsigned int doubleByte     : 2;
        unsigned int isUrl          : 1;
        unsigned int isNothing      : 1;
    } f;
};

struct Cell
{
    unsigned char byte;
    CellAttribute attr;
};

}   // namespace BBS

}   // namespace UJ

#endif // GLOBALS_H
