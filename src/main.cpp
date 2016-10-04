#include <iostream>
#include "../include/Socket.hpp"
#include "../include/SocketManager.hpp"

int main (int argc, char** argv)
{
    analyzer::net::SocketSSL sock;
    if (sock.IsError()) {
        std::cout << "Socket fail..." << std::endl;
    }

    if (!sock.SetHttpOnlyProtocol()) {
        std::cout << "Secure ciphers fail..." << std::endl;
    }
    if (!sock.SetOnlySecureCiphers()) {
        std::cout << "Secure ciphers fail..." << std::endl;
    }

    sock.Connect("www.google.ru");
    //sock.Connect("habrahabr.ru");
    if (sock.IsError()) {
        std::cout << "Connection fail..." << std::endl;
    }

    std::cout << sock.CheckSocketState(3000) << std::endl;

    char buff[] = "GET / HTTP/1.1\r\nHost: www.google.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    sock.Send(buff, strlen(buff));
    if (sock.IsError()) {
        std::cout << "Send fail..." << std::endl;
    }

    char buff_r[DEFAULT_BUFFER_SIZE];
    int32_t len = sock.RecvToEnd(buff_r, DEFAULT_BUFFER_SIZE);
    if (sock.IsError()) {
        std::cout << "Recv fail..." << std::endl;
    }
    std::cout << len << std::endl;

    sock.Close();



    //analyzer::net::NonBlockSocketManager net;
    //char ya[] = "GET / HTTP/1.1\r\nHost: ya.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //pthread_t t_1 = net.Add("ya.ru", ya, strlen(ya));

    //char rbc[] = "GET / HTTP/1.1\r\nHost: www.rbc.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //pthread_t t_2 = net.Add("www.rbc.ru", rbc, strlen(rbc));

    //char keddr[] = "GET / HTTP/1.1\r\nHost: keddr.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //net.Add("keddr.com", keddr, strlen(keddr));

    //char novaya[] = "GET / HTTP/1.1\r\nHost: www.novayagazeta.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //net.Add("www.novayagazeta.ru", novaya, strlen(novaya));

    //char twirpx[] = "GET / HTTP/1.1\r\nHost: www.twirpx.com\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //net.Add("www.twirpx.com", twirpx, strlen(twirpx));

    //char newtimes[] = "GET / HTTP/1.1\r\nHost: newtimes.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //net.Add("newtimes.ru", newtimes, strlen(newtimes));

    //char scanlibs[] = "GET / HTTP/1.1\r\nHost: scanlibs.com\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    //net.Add("scanlibs.com", scanlibs, strlen(scanlibs));


    //net.Add("fe80::e1c5:45c0:1d03:8e38%eth0", ya, sizeof(ya), 443, TCPv6);
    //net.WaitAll();
    //analyzer::net::data_t data_1 = net.GetData(t_1);
    //analyzer::net::data_t data_2 = net.GetData(t_2);
    return EXIT_SUCCESS;
}
