Welcome to UdpLibrarly! (Tizen, Window)
===================


Tizen, Window UDPLibrary란
----------


Tizen, Window UDP Library 는 Tizen - Tizen, Window - Tizen, Window - Windows의 신뢰성있는 p2p 통신을 지원하는 라이브러리 입니다. 중계 서버를 통해 통신하기 때문에 각자의 ID만 가지고 p2p 통신을 할 수 있습니다.

window  QT      .

------------
이 라이브러리에서 지원하는 기능은 다음과 같습니다.

> **지원하는 기능:**

> - Local Area Network에서의 통신
> - UDP Hole Punching 을 통한 Wide Area Network에서의 통신
> - Relay Server을 통한 Wide Area Network에서의 통신

------------
이 라이브러리는 서버를 통해 통신하기 때문에 서버를 실행할 필요가 있습니다.
 사용순서는 다음과 같습니다.
 
> **사용 순서:**
> 
> 1. 중계 서버를 설정하고 시작합니다.
>2. 클라이언트를 중계서버와 연결하고 통신할 상대방을 찾습니다.
>3. 상대방을 찾으면 라이브러리에서 제공되는 API를 통해 통신을 합니다

---------
#### 사용자 인증 방법
이 라이브러리에서 지원하는 사용자 인증 방법은 3가지로 아래와 같습니다.

> **인증 방법:**

> - 서버에서 한 클라이언트의 인증 토큰 생성 후 해당 토큰으로 인증
> - 특정 ID값을 양쪽 클라이언트에서 인증
> - ID와 PW를 양쪽 클라이언트에서 인증

#### 클라이언트 통신 API 설명
사용자는 send 와 onReceived를 사용할 수 있다.

send 함수 사용 예시
```cpp
// Send 함수
String token = "사용자 인증 토큰";//인증 과정에서 받아온 토큰
int myCallbackFunction(){//해당 함수가 끝난 후 실행할 콜백 함수
......
};
send("보낼 메시지");//메시지 보내기
```

onReceived 함수 사용 예시
```cpp
// 리스너 함수를 등록하여 패킷을 받았을 때 리스너 함수를 실행한다.
int myCallbackFunction(String token){//패킷을 받으면 실행할 콜백 함수
......
};
onReceived(myCallbackFunction);//메시지 보내기
```
