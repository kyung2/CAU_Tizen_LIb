
# Tizen 라이브러리

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

해당 라이브러리가 지원하는 함수는 다음과 같습니다.
```cpp
extern int tul_init(char * server_ip,int port);

extern int tul_bind_port(int port);

extern int tul_send(char * message,int message_len);

extern int tul_connect(char * token,char * id);

extern int tul_start_listen(Ecore_Thread_Notify_Cb callback, void * data);

extern int tul_stop_listen();

extern int tul_get_time_id(char ** id);
```
각 함수들에 대한 설명

- tul_init 
```cpp
//라이브러리에서 연결할 서버의 ip와 포트를 입력한다.
tul_init("127.0.0.1",2323);
```
- tul_bind_port
```cpp
//라이브러리에서 사용할 특정 포트를 입력한다.
tul_bind_port(2536);
```
- tul_send
```cpp
// Send 함수
//보낼 메시지와 메시지의 길이를 입력하면 연결된 상대방에게 패킷을 보낸다.
tul_send("message",8);
```
- tul_connect
```cpp
// 연결하는 함수
// 상대방의 방 이름에 해당하는 token과 id를 입력하여 연결한다.
tul_connect("ABCD","a123");
```
- tul_start_listen
```cpp
// Listen 콜백을 사용하여 async listen을 실행한다.
// Listen 콜백을 사용하지 않으려면 NULL값을 인자로 준다.
char * data = "this is data";
tul_start_listen(my_callback,(void *)data);
```

- tul_stop_listen
```cpp
// 위에서 돌리는 listen 쓰레드를 종료한다.
tul_stop_listen();
```
- tul_get_time_id
```cpp
// 시간에 따른 8자리 임의의 string 을 만들어준다.
//사용 후 반드시 free 해주어야 한다.
char * my_id;
tul_get_time_id(my_id);
```
