#ifndef UJQXTESTUTILITIES_H
#define UJQXTESTUTILITIES_H

#include <QObject>
#include <cstdio>
#include <iostream>
#include <QDebug>
#include <QTextStream>
using std::endl;

namespace UJ
{

namespace Qx
{

class Tester : public QObject
{
    Q_OBJECT

public:
    explicit Tester(QObject *parent) : QObject(parent)
    {
        _cout = new QTextStream(::stdout, QIODevice::WriteOnly);
    }
    virtual ~Tester()
    {
        delete _cout;
    }

protected:
    QTextStream *_cout;
};

}   // namespace Qx

}   // namespace UJ

#endif  // UJQXTESTUTILITIES_H
