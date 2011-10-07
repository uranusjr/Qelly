#ifndef CONNECTIONWRAPPER_H
#define CONNECTIONWRAPPER_H

class qint16;
class QByteArray;
class QString;

namespace UJ
{

namespace Connection
{

class Site;

template <class T>
class Wrapper
{
public:
    Wrapper(T *connection)
    {
        _connection = 0;
        setConnection(connection);
    }
    virtual ~Wrapper()
    {
        delete _connection;
    }

private:
    T *_connection;

public: // Setter & getters
    inline T *connection()
    {
        return _connection;
    }
    inline void setConnection(T *connection)
    {
        if (_connection)
            delete _connection;
        _connection = connection;
    }

public: // Relaying public functions
    inline bool connectTo(Site *s)
    {
        return _connection->connectTo(s);
    }
    inline bool connectTo(QString &address)
    {
        return _connection->connectTo(address);
    }
    inline bool connectTo(QString &address, qint16 port)
    {
        return _connection->connectTo(address, port);
    }
    inline void close()
    {
        return _connection->close();
    }
    inline void reconnect()
    {
        return _connection->reconnect();
    }
    inline void sendBytes(QByteArray bytes)
    {
        return _connection->sendBytes(bytes);
    }

public: // Relaying setters & getters
    inline Site *site()
    {
        return _connection->site();
    }
    inline void setSite(Site *site)
    {
        return _connection->setSite(site);
    }
    inline QString name()
    {
        return _connection->name();
    }
    inline void setName(QString &name)
    {
        return _connection->setName(name);
    }
    inline QString address()
    {
        return _connection->address();
    }
    inline void setAddress(QString &address)
    {
        return _connection->setAddress(address);
    }
    inline bool isConnected()
    {
        return _connection->isConnected();
    }
    inline void setConnected(bool isConnected)
    {
        return _connection->setConnected(isConnected);
    }
    inline bool isProcessing()
    {
        return _connection->isProcessing();
    }
    inline void setProcessing(bool isProcessing)
    {
        return _connection->setProcessing(isProcessing);
    }
};

}   // namespace Connection

}   // namespace UJ

#endif // CONNECTIONWRAPPER_H
