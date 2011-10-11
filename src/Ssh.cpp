#include "Ssh.h"

namespace UJ
{

namespace Connection
{

Ssh::Ssh(QObject *parent) : AbstractConnection(parent)
{
    _type = TypeSsh;
}

}   // namespace Connection

}   // namespace UJ
