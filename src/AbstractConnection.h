#ifndef ABSTRACTCONNECTION_H
#define ABSTRACTCONNECTION_H

#include <QObject>

namespace UJ
{

class Terminal;

namespace Connection
{

class Site;

enum Type
{
    TypeUnKnown,
    TypeTelnet,
    TypeSsh
};

class AbstractConnection : public QObject
{
    Q_OBJECT

public:
    explicit AbstractConnection(QObject *parent = 0);
    virtual bool connectTo(Site *s) = 0;
    virtual bool connectTo(QString &address, qint16 port) = 0;
    void dealloc();

public slots:
    virtual void close() = 0;
    virtual void reconnect() = 0;
    virtual void sendBytes(QByteArray bytes) = 0;

protected:
    Type _type;
    Site *_site;
    QString _name;
    QString _address;
    // NOTE: Need an image member for tab icon...
    //       Haven't decided which class, maybe QIcon?
    bool _isConnected;
    bool _isProcessing;

protected slots:
    virtual void processBytes(QByteArray bytes) = 0;

signals:
    void connected();
    void disconnected();
    void processedBytes(QByteArray bytes);

public: // Getters & Setters
    virtual inline Site *site()
    {
        return _site;
    }
    virtual void setSite(Site *site);
    virtual inline QString name()
    {
        return _name;
    }
    virtual inline void setName(QString &name)
    {
        _name = name;
    }
    virtual inline QString address()
    {
        return _address;
    }
    virtual inline void setAddress(QString &address)
    {
        _address = address;
    }
    virtual inline bool isConnected()
    {
        return _isConnected;
    }
    virtual inline void setConnected(bool isConnected)
    {
        // NOTE: Should change the icon image here...
        _isConnected = isConnected;
    }
    virtual inline bool isProcessing()
    {
        return _isProcessing;
    }
    virtual inline void setProcessing(bool isProcessing)
    {
        _isProcessing = isProcessing;
    }
};

}   // namespace Connection

}   // namespace UJ

#endif // ABSTRACTCONNECTION_H
