// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include "../include/framework/AnalyzerApi.hpp"


int main(void)
{
    analyzer::task::TaskManager net;
    char ya[] = "GET / HTTP/1.1\r\nHost: ya.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_1 = net.Add(ya, strlen(ya), "ya.ru");

    char rbc[] = "GET / HTTP/1.1\r\nHost: www.rbc.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_2 = net.Add(rbc, strlen(rbc), "www.rbc.ru");

    char novaya[] = "GET / HTTP/1.1\r\nHost: www.novayagazeta.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_3 = net.Add(novaya, strlen(novaya), "www.novayagazeta.ru");

    char twirpx[] = "GET / HTTP/1.1\r\nHost: www.twirpx.com\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_4 = net.Add(twirpx, strlen(twirpx), "www.twirpx.com");

    char newtimes[] = "GET / HTTP/1.1\r\nHost: newtimes.ru\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_5 = net.Add(newtimes, strlen(newtimes), "newtimes.ru");

    char fanserials[] = "GET / HTTP/1.1\r\nHost: fanserials.tv\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_6 = net.Add(fanserials, strlen(fanserials), "fanserials.tv");

    char keddr[] = "GET / HTTP/1.1\r\nHost: keddr.com\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_7 = net.Add(keddr, strlen(keddr), "keddr.com");

    char scanlibs[] = "GET / HTTP/1.1\r\nHost: scanlibs.com\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    pthread_t t_8 = net.Add(scanlibs, strlen(scanlibs), "scanlibs.com");


    //net.Add(ya, sizeof(ya), "fe80::e1c5:45c0:1d03:8e38%eth0", 443, TCPv6);
    net.WaitAll();
    analyzer::task::data_t data_1 = net.GetData(t_1);
    analyzer::task::data_t data_2 = net.GetData(t_2);
    analyzer::task::data_t data_3 = net.GetData(t_3);
    analyzer::task::data_t data_4 = net.GetData(t_4);
    analyzer::task::data_t data_5 = net.GetData(t_5);
    analyzer::task::data_t data_6 = net.GetData(t_6);
    analyzer::task::data_t data_7 = net.GetData(t_7);
    analyzer::task::data_t data_8 = net.GetData(t_8);

    return EXIT_SUCCESS;
}
