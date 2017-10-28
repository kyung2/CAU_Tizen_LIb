#ifndef UNTITLED_H
#define UNTITLED_H
#include <QStringList>
#include <QHostAddress>
#include <QUdpSocket>
#include <QByteArray>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QThread>
#include <QtConcurrent/qtconcurrentrun.h>

struct address{
    QHostAddress LocalAddress;
    int LocalPort = 0;
    QHostAddress PublicAddress;
    int PublicPort = 0;
};

enum CONNECT{
    LOCAL_CONNECT = 1,
    PUBLIC_CONNECT = 2,
    RELAY_CONNECT = 3,
    NOT_CONNECTED = 4
};

class connectLibrary :public QObject
{
    Q_OBJECT

public:
    connectLibrary(QObject *parent = 0);

public slots:
    void connect_other();

signals:
    void writeReady(char);
    void finished();
    void successConnection(int);

};

class UdpLibrary :public QObject
{
    Q_OBJECT
public:
    static UdpLibrary* singleTonInstance;

    UdpLibrary(QObject *parent = 0);
    ~UdpLibrary();

    static UdpLibrary* getInstance();


    int UdpLibrary::init(QString server, int port);
    int bindSocket(int port);

    int enroll(QByteArray token, QByteArray id);
    int connects(QByteArray token, QByteArray id);
    void send(QString datagram);


signals:
    void sendToUser(QStringList message);
    void connectState(bool);
//    void started();

public slots:
    void set_listen_callback();
    void checkConnect(char);
    void checkState(int);

private:

    void send(QHostAddress address,int port, QString datagram);

    QThread *thread;
    QThread * connectThread;
    connectLibrary *cl;

    void checkData(QString data);

    QUdpSocket * udpSocket;
    QStringList message;
};


#endif // UNTITLED_H
