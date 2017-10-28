#include "udplibrary.h"
#include <QDebug>
#include <QThread>

address myAddress;
address clientAddress;
address serverAddress;

int state = CONNECT::NOT_CONNECTED;

int checkIP;

bool recvf = false;
bool recvt = false;
bool start = false;

UdpLibrary::UdpLibrary(QObject *parent)
{
    qDebug("udp start");
    udpSocket = new QUdpSocket(this);
    if(!udpSocket){
        //qDebug() << "socket error";
    }
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

     for(int nIter=0; nIter<list.count(); nIter++)
      {
          if(!list[nIter].isLoopback())
              if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ){
                  myAddress.LocalAddress = list[nIter].toString();
                  //qDebug() << list[nIter].toString();
                  break;
              }
      }

    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(set_listen_callback()));

    connectThread = new QThread();
    cl = new connectLibrary();
    connect(connectThread,SIGNAL(started()), cl,SLOT(connect_other()));
    connect(cl,SIGNAL(successConnection(int)), this, SLOT(checkState(int)));
    connect(cl,SIGNAL(finished()),connectThread,SLOT(quit()));
    connect(cl,SIGNAL(writeReady(char)), this, SLOT(checkConnect(char)));

    cl->moveToThread(connectThread);
}

UdpLibrary::~UdpLibrary(){
    delete udpSocket;
    delete cl;
    delete connectThread;
}

int UdpLibrary::init(QString server, int port)
{
    //qDebug("start init");

    QHostAddress serverIP = QHostAddress::QHostAddress(server);

    //qDebug(server.toUtf8());
    serverAddress.LocalAddress = serverIP;
    qDebug(serverAddress.LocalAddress.toString().toUtf8());
    serverAddress.LocalPort = port;
    qDebug("end init");
    return 0;
}

void UdpLibrary::checkConnect(char target){
    if(target == 'l')
        if(!recvf)
            send(clientAddress.LocalAddress,clientAddress.LocalPort,"l|f");
        else
            send(clientAddress.LocalAddress,clientAddress.LocalPort,"l|t");
    else if(target == 'g')
        if(!recvf)
            send(clientAddress.PublicAddress,clientAddress.PublicPort,"g|f");
        else
            send(clientAddress.PublicAddress,clientAddress.PublicPort,"g|t");
    else
        if(!recvf)
            send(serverAddress.LocalAddress,serverAddress.LocalPort,"r|f");
        else
            send(serverAddress.LocalAddress,serverAddress.LocalPort,"r|t");
}

UdpLibrary * UdpLibrary::singleTonInstance= NULL;

int UdpLibrary::enroll(QByteArray token, QByteArray id){

    //qDebug("library test");
    udpSocket -> connectToHost(serverAddress.LocalAddress, serverAddress.LocalPort, QIODevice::ReadWrite);
    // data from external function
    QByteArray datagram = "";
    datagram = "e|" + token + "|" + id + "|";
    datagram += myAddress.LocalAddress.toString().toUtf8() + "|" + QByteArray::number(myAddress.LocalPort) + "|";
    udpSocket->write(datagram);
    udpSocket->disconnectFromHost();
    this->bindSocket(myAddress.LocalPort);
    return 0;
}

int UdpLibrary::connects(QByteArray token, QByteArray id){
    udpSocket -> connectToHost(serverAddress.LocalAddress, serverAddress.LocalPort, QIODevice::ReadWrite);
    // data from external function
    QByteArray datagram = "";
    datagram = "e|" + token + "|" + id + "|";
    datagram += myAddress.LocalAddress.toString().toUtf8() + "|" + QByteArray::number(myAddress.LocalPort) + "|";
    udpSocket->write(datagram);

    udpSocket->disconnectFromHost();
    this->bindSocket(myAddress.LocalPort);
    return 0;
}


void UdpLibrary::send(QHostAddress address,int port, QString datagram){
    udpSocket -> connectToHost(address, port, QIODevice::ReadWrite);
    udpSocket->write(datagram.toUtf8());
    //qDebug() << "mySendData : " << datagram.toUtf8() << address;
    if (datagram[0] != 'l' && datagram[0] != 'c' && datagram[0] != 'g' && datagram[0] != 'r' )
        datagram = "m|" + datagram;
    udpSocket->disconnectFromHost();
    this->bindSocket(myAddress.LocalPort);
}

void UdpLibrary::send(QString datagram){
    QHostAddress address;
    int port;
    if(state == CONNECT::LOCAL_CONNECT){
        address = clientAddress.LocalAddress;
        port = clientAddress.LocalPort;
    }
    else if(state == CONNECT::PUBLIC_CONNECT){
        address = clientAddress.PublicAddress;
        port = clientAddress.PublicPort;
    }
    else if(state == CONNECT::RELAY_CONNECT){
        address = serverAddress.LocalAddress;
        port = serverAddress.LocalPort;
    }
    else{
        return;
    }
    udpSocket -> connectToHost(address, port, QIODevice::ReadWrite);
    udpSocket->write(datagram.toUtf8());
    //qDebug() << "mySendData : " << datagram.toUtf8() << address;
    datagram = "m|" + datagram;
    udpSocket->disconnectFromHost();
    this->bindSocket(myAddress.LocalPort);
}

void UdpLibrary::set_listen_callback(){
    //qDebug("recv success");
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();

        checkData(datagram.data());
        //processTheDatagram(datagram);

        //return datagram.data();
    }
}

void UdpLibrary::checkData(QString data){
    qDebug(data.toUtf8());
    message = data.split("|");      // for example, message is "hello|world|"
    qDebug() << message.count();    // count = 3, message[0] = "hello", message[1] = "world" ,message[2] = ""
    if(!message.isEmpty()){
        if(message[0] == "c" && message.count() == 8){
            myAddress.PublicAddress = message[1]; myAddress.PublicPort = (message[2]).toInt();
            clientAddress.PublicAddress = message[3]; clientAddress.PublicPort = (message[4]).toInt();
            clientAddress.LocalAddress = message[5]; clientAddress.LocalPort = (message[6]).toInt();
            connectThread->start();
        }
        else if(message[0] == "m"){
            message.removeFirst();
            emit sendToUser(message);
        }
        else if(message[0] == "l"){
            checkIP = 1;
            if (message[1] == "f"){
      //          qDebug() << "0 : " << message[0] << "1 : " << message[1];
                recvf = true;
            }
            else if (message[1] == "t")
                recvt = true;
        }

        else if(message[0] == "g"){
            checkIP = 2;
            if (message[1] == "f"){
        //     qDebug() << "0 : " << message[0] << "1 : " << message[1];
              recvf = true;
            }
            else if (message[1] == "t")
                recvt = true;
        }

        else if(message[0] == "r"){
            checkIP = 3;
            if (message[1] == "f"){
          //   qDebug() << "0 : " << message[0] << "1 : " << message[1];
              recvf = true;
            }
            else if (message[1] == "t")
                recvt = true;
        }

    }
}

int UdpLibrary::bindSocket(int port){
    myAddress.LocalPort = port;
    udpSocket->bind(myAddress.LocalPort,QUdpSocket::ShareAddress);
    return 0;
}

UdpLibrary* UdpLibrary::getInstance(){
   if(UdpLibrary::singleTonInstance == 0){
        UdpLibrary::singleTonInstance = new UdpLibrary();
    }else if(UdpLibrary::singleTonInstance != 0){

    }
    return UdpLibrary::singleTonInstance;
}

connectLibrary::connectLibrary(QObject *parent){

}

void UdpLibrary::checkState(int state){
    if(state == CONNECT::NOT_CONNECTED){
        emit connectState(false);
        return;
    }
    emit connectState(true);
}

void connectLibrary::connect_other(){

    //qDebug() << "connect start";
    //qDebug() << "currentState : " << connectState;
    recvf = false;
    recvt = false;
    state = CONNECT::NOT_CONNECTED;
    int i;
        for(i=0;i<20;i++){										// 로컬 연결 시도
            if(!recvf){
                emit writeReady('l');
            }
            QThread::usleep(5000);
        }
        QThread::usleep(200000);
        if(recvt && checkIP == 1){
            state = CONNECT::LOCAL_CONNECT;				//현재 연결을 LOCAL 로 설정
      //      qDebug() << "currentState : " << state;
        }
        else{
            state = CONNECT::NOT_CONNECTED;					//글로벌 연결 시도
            recvf = false;
            for(i=0;i<20;i++){										// 로컬 연결 시도
                if(!recvf){
                    emit writeReady('g');			//연결안되면 l|f
                }
                QThread::usleep(5000);
            }
            QThread::usleep(200000);
            if(recvt && checkIP == 2){
                state = CONNECT::PUBLIC_CONNECT;
        //        qDebug() << "currentState : " << state;
            }
            else{

                state = CONNECT::NOT_CONNECTED;					//글로벌 연결 시도
                recvf = false;
                for(i=0;i<100;i++){
          //          qDebug() << "recvf : " << recvf;
                    if(!recvf){
                        emit writeReady('r');			//연결안되면 l|f
                    }
                    QThread::usleep(6000);
                }
                QThread::usleep(200000);
            }
            if(recvt && checkIP == 3){
                state = CONNECT::RELAY_CONNECT;
            //    qDebug() << "currentState : " << state;
            }
        }
        //qDebug() << "currentState : " << state;
        emit successConnection(state);
        emit finished();
}
