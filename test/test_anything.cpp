// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/Api.hpp"
#include "../include/analyzer/Common.hpp"

using namespace analyzer::common::text;


bool GetNameWithNextVolume (std::string& name) noexcept;


int32_t main (int32_t size, char** data)
{
    /*const std::size_t value_1 = 10;
    std::cout << getHexValue(value_1) << std::endl;

    const std::size_t value_2 = 923876706450976023;
    std::cout << getHexValue(value_2, sizeof(std::size_t)) << std::endl;

    const unsigned char value_3[3] = { 0, 4, 15 };
    std::cout << getHexString(value_3, sizeof(value_3)) << std::endl;*/

    /*std::string test1("/dndsgndrn/abcde_volume1.log");
    GetNameWithNextVolume(test1);
    std::cout << test1 << std::endl;

    std::string test2("/dndsgndrn/abcde_volume1");
    GetNameWithNextVolume(test2);
    std::cout << test2 << std::endl;

    std::string test3("/dndsgndrn/abcde_volume456.log");
    GetNameWithNextVolume(test3);
    std::cout << test3 << std::endl;

    std::string test4("/dndsgndrn/abcde_volume1024");
    GetNameWithNextVolume(test4);
    std::cout << test4 << std::endl;

    std::string test7("/dndsgndrn/abcde_volume.log");
    GetNameWithNextVolume(test7);
    std::cout << test7 << std::endl;
    GetNameWithNextVolume(test7);
    std::cout << test7 << std::endl;

    std::string test8("/dndsgndrn/abcde_volume");
    GetNameWithNextVolume(test8);
    std::cout << test8 << std::endl;
    GetNameWithNextVolume(test8);
    std::cout << test8 << std::endl;

    std::string test9("/dndsgndrn/abcde_volumeq");
    GetNameWithNextVolume(test9);
    std::cout << test9 << std::endl;
    GetNameWithNextVolume(test9);
    std::cout << test9 << std::endl;
    GetNameWithNextVolume(test9);
    std::cout << test9 << std::endl;*/

    std::string test1("dndsgndrn");
    GetNameWithNextVolume(test1);
    std::cout << test1 << std::endl;

    std::string test2("/dndsgndrn/abcde.lllg");
    GetNameWithNextVolume(test2);
    std::cout << test2 << std::endl;

    std::string test3("/dndsgndrn/abcde");
    GetNameWithNextVolume(test3);
    std::cout << test3 << std::endl;

    std::string test4("/dndsgn.drn/abcde");
    GetNameWithNextVolume(test4);
    std::cout << test4 << std::endl;

    return EXIT_SUCCESS;
}


bool GetNameWithNextVolume (std::string& name) noexcept
{
    try
    {
        std::size_t volumeSign = name.find("_volume");
        if (volumeSign != std::string::npos)
        {
            volumeSign += 7;
            if (volumeSign == name.size()) {
                name.append("1.log");
            }
            else if (isNumber(name.at(volumeSign)) == false) {
                name.insert(volumeSign, 1, '1');
            }
            else {
                std::size_t endPlace = 0;
                std::size_t volumeNumber = std::stoul(name.substr(volumeSign), &endPlace, 10);
                name.replace(volumeSign, endPlace, std::to_string(++volumeNumber));
            }
        }
        else
        {
            const std::size_t dot = name.find_last_of('.');
            const std::size_t pathDelimiter = name.find_last_of('/');
            if (dot == std::string::npos) {
                name.append("_volume1.log");
            }
            else if (pathDelimiter != std::string::npos && dot > pathDelimiter) {
                name.insert(dot, "_volume1");
            }
            else {
                name.append("_volume1.log");
            }
        }
    }
    catch (const std::exception& err) {
        std::cout << "Error with name: " << name << "   " << err.what() << std::endl;
        return false;
    }
    return true;
}

