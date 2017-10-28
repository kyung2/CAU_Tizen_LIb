#include "udp_simple_socket.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
struct sockaddr_in temp_addr;
bool sock_set = false;
int my_listen_callback(struct sockaddr_in sender, const std::string& msg){
    std::cout<<msg<<"callback!!!!"<<std::endl;
    if(!sock_set){
        temp_addr= sender; 
        sock_set=true;
    }
    else{
        udp_simple_socket::getInstance()->sendSync(msg);
    }
}

int main(){
    udp_simple_socket * socket =udp_simple_socket::getInstance();
    socket -> bind_port(2322);
    socket -> listen();
    socket -> set_listen_callback_func(my_listen_callback);
    socket -> sendSync("127.0.0.1",51415,"hi");
    while(1){
        sleep(1);
    }
    return 0;
}
