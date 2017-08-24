// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html
http://[1080:0:0:0:8:800:200C:417A]/index.html
http://[3ffe:2a00:100:7031::1]
http://[1080::8:800:200C:417A]/foo
http://[::192.9.5.5]/ipng
http://[::FFFF:129.144.52.38]:80/index.html
http://[2010:836B:4179::836B:4179]

http://142.42.1.1/
http://142.42.1.1:8080/
http://223.255.255.254
http://userid:password@example.com:8080
http://userid:password@example.com:8080/
https://www.example.com/foo/?bar=baz&inga=42&quux
http://www.example.com/wpstyle/?p=364
http://foo.com/blah_blah_(wikipedia)_(again)
http://code.google.com/events/#&product=browser
*/
#include <string>
#include <iostream>

#include "../include/analyzer/Api.hpp"


int main(void)
{
    const std::string ref = "https://www.google.ru::443/first/second?sourceid=chrome&espv=2&ie=UTF-8#newwindow=1&*";

}
