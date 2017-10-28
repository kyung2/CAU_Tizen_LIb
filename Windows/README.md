
# Window 라이브러리

> **사용 순서:**
>1. 어플리케이션 시작할때 라이브러리 초기화(서버 아이피 및 포트 설정)
>2. 클라이언트를 중계서버와 연결하고 통신할 상대방을 찾습니다.
>3. 상대방을 찾으면 라이브러리에서 제공되는 API를 통해 통신을 합니다

---------
#### 사용자 인증 방법
현재 이 라이브러리에서 지원하는 사용자 인증 방법은 2가지로 아래와 같습니다.

> **인증 방법:**
> - 특정 token과 ID값을 양쪽 클라이언트에서 인증
> - 서버에서 한 클라이언트의 인증 토큰 생성 후 해당 토큰으로 인증 -- 미지원 

#### 적용 방법

해당 라이브러리가 지원하는 변수 및 함수는 다음과 같습니다.
```cpp

static UdpLibrary* getInstance();

int init(QString server, int port);

int bindSocket(int port);

void send(QString datagram);

int enroll(QByteArray token, QByteArray id);

int connects(QByteArray token, QByteArray id);

siganls: 
  void sendToUser(QStringList message);

  void connectState(bool);
```
각 변수 및 함수들에 대한 설명


- getInstance
```cpp
// QUdpSocket을 싱글톤으로 만들어 주기 위한 함수입니다.
UdpLibrary* UDP;
UDP= UdpLibrary::getInstance();
```

- init 
```cpp
//라이브러리에서 연결할 서버의 ip와 포트를 입력합니다.
UDP->init("127.0.0.1", 1234);
```
- bindSocket
```cpp
//라이브러리에서 사용할 특정 포트를 입력합니다.
UDP->bindSocket(3456);
```
- send
```cpp
// Send 함수
//보낼 메시지를 입력하면 연결된 상대방에게 패킷을 보냅니다.
send("message");
```
- connect, enroll
```cpp
// 연결하는 함수
// 상대방의 방 이름에 해당하는 token과 id를 입력하여 연결합니다.
connect("ABCD","a123");
enroll("ABCD","a123");
```
- sendToUser
```cpp
// 연결된 클라이언트가 메세지를 보내면 시그널이 발생합니다.
// 슬롯함수를 등록 해놓고 connect로 연결하면 시그널이 발생될 때마다 슬롯함수가 실행 됩니다.
// slots함수로  receiveMessage(QStringList)을 선언한 경우,
private slots:
  void receiveMessage(QStringList){
  // 메세지를 읽고 동작할 내용
  }
QObject::connect(UDP,SIGNAL(sendToUser(QStringList)),this,SLOT(receiveMessage(QStringList)));
```

- connectState
```cpp
// connect요청이 다른 클라이언트나 서버로 부터 온 경우 시그널이 발생합니다.
// 슬롯함수를 등록 해놓고 connect로 연결하면 시그널이 발생될 때마다 슬롯함수가 실행 됩니다.

// 연결 되었을 경우에 true를 그렇지 않은 경우 false를 반환합니다.
// slots함수로 checkConnect(bool)을 선언한 경우,
private slots:
  void checkConnect(bool){
  }
QObject::connect(UDP,SIGNAL(connectState(bool)),this,SLOT(checkConnect(bool)));
```

#### 사용 예제

> 해당 라이브러리 함수는 다음과 같은 방법으로 사용합니다.
> socket 사용시 순서가 변경되면 문제가 생길 수 있으니 다음과 같은 절차를 지켜주세요.

> 메세지를 받은 후 처리하기 위해서는 receiveMessage 함수 정의가 필요합니다.
>
> connect 상태를 확인하고 싶다면 checkConnect 함수를 정의해주세요.

> enroll() 대신 connect()함수 사용하여도 됩니다.
> connect가 성공한 경우 다른 클라이언트에 메세지를 보내기 위해서 send 함수를 사용할 수 있습니다.

```cpp
UDPLibrary *UDP;  // 라이브러리 사용을 위한 변수선언. header file private으로 선언추천

// 다음 부분들은 시그널-슬롯 연결을 위해 어플리케이션(mainwindow나 dialog, widget 등) 생성자에 선언해주세요.
UDP= UdpLibrary::getInstance();
QObject::connect(UDP,SIGNAL(sendToUser(QStringList)),this,SLOT(receiveMessage(QStringList)));
QObject::connect(UDP,SIGNAL(connectState(bool)),this,SLOT(checkConnect(bool)));

// server와 연결하고싶은부분에 적용해주세요. (ex. button event, app constructor)
UDP->init("222.108.245.162", 23272);
UDP->bindSocket(3456);
UDP->enroll(token,id); // token을 알아야 연결가능

```
