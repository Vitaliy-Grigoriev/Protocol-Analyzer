#include <iostream>
#include "../include/Socket.hpp"
#include "../include/SocketManager.hpp"

int main (int argc, char** argv)
{
    analyzer::net::NonBlockSocketManager net;
    char ya[] = "GET / HTTP/1.1\r\nHost: ya.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_1 = net.Add("ya.ru", ya, strlen(ya));

    char rbc[] = "GET / HTTP/1.1\r\nHost: www.rbc.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_2 = net.Add("www.rbc.ru", rbc, strlen(rbc));

    char novaya[] = "GET / HTTP/1.1\r\nHost: www.novayagazeta.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_3 = net.Add("www.novayagazeta.ru", novaya, strlen(novaya));

    char twirpx[] = "GET / HTTP/1.1\r\nHost: www.twirpx.com\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_4 = net.Add("www.twirpx.com", twirpx, strlen(twirpx));

    char newtimes[] = "GET / HTTP/1.1\r\nHost: newtimes.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_5 = net.Add("newtimes.ru", newtimes, strlen(newtimes));

    //net.Add("fe80::e1c5:45c0:1d03:8e38%eth0", ya, sizeof(ya), 443, TCPv6);
    net.WaitAll();
    analyzer::net::data_t data_1 = net.GetData(t_1);
    analyzer::net::data_t data_2 = net.GetData(t_2);
    analyzer::net::data_t data_3 = net.GetData(t_3);
    analyzer::net::data_t data_4 = net.GetData(t_4);
    analyzer::net::data_t data_5 = net.GetData(t_5);
    return EXIT_SUCCESS;
}

