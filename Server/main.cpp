#include "udp_simple_socket.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <vector>
using namespace std;
struct user {
    string token;
    string id;
    struct sockaddr_in  global;
    struct sockaddr_in  local;
};

vector<struct user> user_vector;

void *  heart_beat_func(void * ){
    while(true){
        vector<struct user> temp_beat;
        vector<struct user>::iterator vi;
        temp_beat.assign( user_vector.begin(), user_vector.end() ); 
        for(vi = temp_beat.begin();vi!=temp_beat.end();vi++){
            udp_simple_socket::getInstance()->sendSync(vi->global,"h|");
        }
        sleep(10);
    }
}

int relay_user(struct sockaddr_in sender,stringstream & ss,char relay_char){
    string message;
    message = ss.str();
    if(relay_char == 'R'){
        message[0]='m';
    }
    vector<struct user>::iterator vi;
    struct user to_connect,me;//연결할놈
    int to_connect_count=0;//연결할놈 카운트
    for(vi = user_vector.begin();vi!=user_vector.end();){
        //TODO ip 비교하고 다르면 연결 refresh
        if(sender.sin_addr.s_addr == vi->global.sin_addr.s_addr&&sender.sin_port==vi->global.sin_port){
            cout<<"find me"<<endl;
            me = *vi;
            cout<<me.token<<"  "<<me.id<<endl;
            break;
        }
        vi++;
    }
    for(vi = user_vector.begin();vi!=user_vector.end();){
        //TODO ip 비교하고 다르면 연결 refresh
        if(vi->token.compare(me.token)==0 && vi->id.compare(me.id)!=0){
            cout<<"find other"<<endl;
            to_connect = *vi;
            break;
        }
        vi++;
    }
    udp_simple_socket::getInstance()->sendSync(to_connect.global,message);
}

int connect_two_user(struct user&user_A,struct user&user_B){//두 유저한테 연결정보 갱신 알려줌
    stringstream to_stream_A,to_stream_B;
    string connect_string("c");
    to_stream_A <<  connect_string;
    to_stream_A <<'|'<<inet_ntoa(user_A.global.sin_addr);
    to_stream_A <<'|'<<ntohs(user_A.global.sin_port);
    to_stream_A <<'|'<<inet_ntoa(user_B.global.sin_addr);
    to_stream_A <<'|'<<ntohs(user_B.global.sin_port);
    to_stream_A <<'|'<<inet_ntoa(user_B.local.sin_addr);
    to_stream_A <<'|'<<ntohs(user_B.local.sin_port)<<'|';
    string to_user_A(to_stream_A.str());
    to_stream_B<<connect_string<<'|';
    to_stream_B<<inet_ntoa(user_B.global.sin_addr);
    to_stream_B<<'|'<<ntohs(user_B.global.sin_port);
    to_stream_B<<'|'<<inet_ntoa(user_A.global.sin_addr);
    to_stream_B<<'|'<<ntohs(user_A.global.sin_port);
    to_stream_B<<'|'<<inet_ntoa(user_A.local.sin_addr);
    to_stream_B<<'|'<<ntohs(user_A.local.sin_port)<<'|';
    string to_user_B(to_stream_B.str());
    cout<<to_user_A<<endl<<to_user_B<<endl;
    udp_simple_socket::getInstance()->sendSync(user_A.global,to_user_A);
    udp_simple_socket::getInstance()->sendSync(user_B.global,to_user_B);
}

int enroll_user(struct sockaddr_in& sender, stringstream & ss){
    string token,id,local_ip,local_port;
    bool temp = getline(ss,token,'|');
    if(temp){
        temp = getline(ss,id,'|');
        if(temp){
            temp = getline(ss,local_ip,'|');
            if(temp){
                temp = getline(ss,local_port,'|');
                if(temp){
                    if(udp_simple_socket::check_address(local_ip,local_port)){
                        cout<< "ip success"<<endl;

                        // 해당 아이디가 리스트에 있는지 검사
                        vector<struct user>::iterator vi;
                        struct user to_connect;//연결할놈
                        int to_connect_count=0;//연결할놈 카운트
                        bool to_refresh = false, to_add_user = true;
                        for(vi = user_vector.begin();vi!=user_vector.end();){
                            bool id_cmp = vi->id.compare(id)==0;
                            bool token_cmp = vi->token.compare(token)==0;
                            if(id_cmp){
                                if(token_cmp){
                                    //TODO ip 비교하고 다르면 연결 refresh
                                    if(sender.sin_addr.s_addr != vi->global.sin_addr.s_addr||sender.sin_port!=vi->global.sin_port){
                                        cout<<"user ip updated"<<endl;
                                        vi = user_vector.erase(vi); //현재 아이피 지우기
                                        to_refresh = true;
                                        continue;
                                    }else{
                                        to_add_user = false;
                                    }
                                }else{
                                    //TODO 연결 해제하고 새로운 토큰에 연결하기
                                    cout<<token<<"disconnect and connect to new"<<endl;
                                    vi = user_vector.erase(vi); //현재 아이피 지우기
                                    to_refresh = true;
                                    continue;
                                }
                            }else{
                                if(token_cmp){
                                    //TODO 현재 연결된 애가 2명이면 reject, 1명이면 연결해주기
                                    if(to_connect_count == 0){
                                        to_connect = *vi;
                                        to_connect_count ++;
                                        to_refresh = true;
                                        cout<<token<<"find one to connect"<<endl;
                                    }else{
                                        //TODO 여기서 return 해주고 해당 토큰이 busy하다고 알려줘야함
                                        cout<<token<<"this token is busy"<<endl;
                                        return 0;
                                    }
                                }
                            }
                            vi++;
                        }

                        struct user temp_user = user();
                        struct sockaddr_in temp_addr =  sockaddr_in();

                        temp_addr.sin_family = AF_INET;
                        temp_addr.sin_addr.s_addr = inet_addr(local_ip.c_str());
                        temp_addr.sin_port = htons(atoi(local_port.c_str()));

                        temp_user.token = token;
                        temp_user.id = id;
                        temp_user.global = sender;
                        temp_user.local = temp_addr;
                        if(to_add_user) user_vector.push_back(temp_user);
                        if(to_refresh&&to_connect_count==1)connect_two_user(temp_user,to_connect);
                    }else{
                        cout<< "ip fail"<<endl;
                    }
                }
            }
        }
    }
    
}

int my_listen_callback(struct sockaddr_in sender, const std::string& msg){
    stringstream ss(msg);
    string s;
    bool temp = getline(ss,s,'|');
    if(temp){
        if(s.compare("e")==0){
            cout << "enroll part" << endl;
            enroll_user(sender,ss);
        }else if(s.compare("r")==0||s.compare("R")==0){
            cout << "relay data" <<endl;
            relay_user(sender,ss,s[0]);
        }
    }
    cout<<"this is callback"<<endl;
}
int main(void)
{   
    pthread_t heart_beat;
    udp_simple_socket * socket =udp_simple_socket::getInstance();
    socket -> bind_port(23272);
    socket -> listen();
    socket -> set_listen_callback_func(my_listen_callback);
    pthread_create(&heart_beat, NULL, heart_beat_func, NULL);
    while(1){
        sleep(1);
    }
    return 0;
}