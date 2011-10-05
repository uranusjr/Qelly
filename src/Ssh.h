#ifndef SSH_H
#define SSH_H

#include "AbstractConnection.h"

namespace UJ
{

namespace Connection
{

class Ssh : public AbstractConnection
{
    Q_OBJECT

public:
    explicit Ssh(QObject *parent = 0);

signals:

public slots:

};

}   // namespace Connection

}   // namespace UJ

#endif // SSH_H
