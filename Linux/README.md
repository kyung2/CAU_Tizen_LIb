# TizenUdpLibrarly

g++ -pthread -c ./udp_simple_socket.cpp

ar rvs my_socket.a udp_simple_socket.o

ar rc libmy_socket.a udp_simple_socket.o