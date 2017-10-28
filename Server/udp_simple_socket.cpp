#include "udp_simple_socket.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

udp_simple_socket* udp_simple_socket::singleTonInstance = NULL;
udp_listen_callback m_listen_callback = NULL;

udp_simple_socket* udp_simple_socket::getInstance(){
    
    if(udp_simple_socket::singleTonInstance==NULL){
        udp_simple_socket::singleTonInstance = new udp_simple_socket();
    }else if(udp_simple_socket::singleTonInstance != NULL){
        
    }
    return udp_simple_socket::singleTonInstance;
}

udp_simple_socket::udp_simple_socket()
{
    f_socket = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
    if(f_socket == -1)
    {
        // freeaddrinfo(f_addrinfo);
        perror("addrinfo fail");
    }  
}
udp_simple_socket::~udp_simple_socket()
{
    close(f_socket);
}

/** \brief Retrieve a copy of the socket identifier.
 *
 * This function return the socket identifier as returned by the socket()
 * function. This can be used to change some flags.
 *
 * \return The socket used by this UDP client.
 */
int udp_simple_socket::get_socket() const
{
    return f_socket;
}

/** \brief Retrieve the port used by this UDP client.
 *
 * This function returns the port used by this UDP client. The port is
 * defined as an integer, host side.
 *
 * \return The port as expected in a host integer.
 */
int udp_simple_socket::get_port() const
{
    return f_port;
}

/** \brief Retrieve a copy of the address.
 *
 * This function returns a copy of the address as it was specified in the
 * constructor. This does not return a canonalized version of the address.
 *
 * The address cannot be modified. If you need to send data on a different
 * address, create a new UDP client.
 *
 * \return A string with a copy of the constructor input address.
 */
// std::string udp_simple_socket::get_addr() const
// {
//     return f_addr;
// }

bool udp_simple_socket::listen()  
{
    int thr_id;

    printf("Before Thread\n"); 
    thr_id = pthread_create(&listen_thread, NULL, udp_simple_socket::startRecv, (void *)this);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }
    pthread_detach(listen_thread);
    return true;
}
bool udp_simple_socket::sendSync(std::string message) const  
{
    pthread_t p_thread;
    int thr_id;
    int status;
    struct two_arg_struct socket_and_message;
    socket_and_message.arg1 = (void *)this;
    socket_and_message.arg2 = (void *)&message;
    // printf("Before Thread\n"); 
    thr_id = pthread_create(&p_thread, NULL, udp_simple_socket::startSend, (void *)&socket_and_message);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    pthread_join(p_thread,(void **)&status);
    return true;
}

bool udp_simple_socket::sendSync(struct sockaddr_in sender, std::string message){
    si_other = sender;
    sendSync(message);
}
bool udp_simple_socket::sendSync(const std::string& addr,int port, std::string message){
    memset(&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr = inet_addr(addr.c_str());
    si_other.sin_port = htons(port);
    slen = sizeof(si_other);
    sendSync(message);
}

void *  udp_simple_socket::startRecv(void * This)
{
    ((udp_simple_socket *)This)->recv();
    return NULL;
}
void *  udp_simple_socket::startSend(void * args)
{
    struct udp_simple_socket::two_arg_struct * temp = (struct udp_simple_socket::two_arg_struct * )args;
    ((udp_simple_socket *)temp->arg1)->send((std::string *)temp->arg2);
    return NULL;
}

void  udp_simple_socket::recv()
{
    char buf[1024];
    int  recv_len;
    int s = f_socket;
    slen = sizeof(si_other); 
     while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
         
        //try to receive some data, this is a blocking call
        memset(buf,0,1024);
        if ((recv_len = recvfrom(s, buf, 1024, 0, (struct sockaddr *) &si_other, (socklen_t*)&slen)) == -1)
        {
            perror("recvfrom fail()");
        }
        
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        printf("Data: %s\n" , buf);
        std::string temp_message = std::string(buf);
        if(m_listen_callback ==NULL){
            udp_simple_socket::sendSync(temp_message);
        }else{
            m_listen_callback(si_other,temp_message);
        }
    }
    return;
}
void udp_simple_socket::send(std::string * message)const
{
    if (sendto(f_socket, message->c_str(), message->length(), 0, (struct sockaddr*) &si_other, slen) == -1)
    {
        perror("sendto fail()");

    }
}
int udp_simple_socket::bind_port(int port)
{
    f_port = port;
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family      = AF_INET;
    my_addr.sin_port        = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if( bind(f_socket, (const struct sockaddr *)&my_addr,sizeof(my_addr)) == -1)
    {
        perror("bind fail");
    }
}
int udp_simple_socket::set_listen_callback_func(udp_listen_callback callback){
    m_listen_callback = callback;
}
int udp_simple_socket::check_address(const std::string& usr_ip,const std::string& port_string){
    int port = atoi(port_string.c_str());
    // cout<<"port is "<< port <<endl;
    if(port==0) return 0;
    struct sockaddr_in sa;  
    int result = inet_pton(AF_INET, usr_ip.c_str(), &(sa.sin_addr));
    return result !=0;
}