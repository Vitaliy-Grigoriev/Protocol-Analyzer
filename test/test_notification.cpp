// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/System.hpp"
#include "../include/analyzer/Notification.hpp"

using namespace analyzer::task;
using namespace analyzer::system;


int32_t main (int32_t size, char** data)
{
    auto notify = allocMemoryForObject<NotificationSubject<uint16_t>>();
    notify->SetValue(116);


    return EXIT_SUCCESS;
}

