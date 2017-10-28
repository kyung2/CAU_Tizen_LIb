#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <string>
typedef int (*udp_listen_callback)(struct sockaddr_in sender,const std::string& msg);
class udp_simple_socket
{
private:
    static udp_simple_socket * singleTonInstance;
    int                 f_socket;
    int                 f_port;
    struct addrinfo *   f_addrinfo;

    struct sockaddr_in  si_other;
    int                 slen;

    pthread_t           listen_thread;

    struct two_arg_struct {
        void * arg1;
        void * arg2;
    };

    udp_listen_callback m_listen_callback;
    udp_simple_socket&  operator=(const udp_simple_socket&);
    static void *       startRecv(void * This);
    static void *       startSend(void * args);
public:
    // static udp_simple_socket&   getInstance();  
    
    udp_simple_socket();
    ~udp_simple_socket();
    static udp_simple_socket*   getInstance();
    int                 get_socket() const;
    int                 get_port() const;
    // std::string         get_addr() const;

    bool                listen();
    bool                sendSync(std::string) const;
    bool                sendSync(struct sockaddr_in sender, std::string message);
    bool                sendSync(const std::string& addr,int port, std::string message);
    // int                 sendAsync(const char *msg, size_t size);

    void                recv();//for문 돌면서 listen
    void                send(std::string * message) const;//send 하기
    int                 bind_port(int port);
    int                 set_listen_callback_func(udp_listen_callback listen_callback);

};