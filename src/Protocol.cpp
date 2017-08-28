// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <regex>

#include "../include/analyzer/Log.hpp"  // In this header file also defined "Common.hpp".
#include "../include/analyzer/Socket.hpp"


#define MAX_URL_LENGTH 2048 // Microsoft Internet Explorer


namespace analyzer::net::protocols
{
    ParseURI::ParseURI (const char* link, const std::size_t size)
            : port(DEFAULT_PORT)
    {
        if (link != nullptr && size > 0)
        {
            // scheme:[//[user:password@]host[:port]][/]path[?query][#fragment]
            // scheme((2)1)  domain((4)3)  path(5)  query((7)6)  fragment((9)8)
            const std::regex RegexURI(R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)");
            std::cmatch Result;
            if ( std::regex_match(link, Result, RegexURI) )
            {
                scheme = Result[2];
                url = Result[4];
                path = Result[5];
                query = Result[7];
                fragment = Result[9];
            }
            else {
                isErrorOccur = true;
                LOG_ERROR("ParseURI: Incorrect input link - '", link, "'.");
                return;
            }

            // Parse different schemes in URLs.
            const std::regex RegexURL_Auth(R"(^((.*)\s*:\s*(.*)\s*@)?([^:]*)?(:(\d*))?)");
            const std::regex RegexURL_IPv4(R"(^([0-9.]*)?(:(\d*))?)");
            const std::regex RegexURL_IPv6(R"(^(\[([0-9a-fA-F:.]+)\])?(:(\d*))?)");




        }
        isErrorOccur = true;
        LOG_ERROR("ParseURI: The input link has zero length.");
    }

    ParseURI::ParseURI (const std::string& link)
            : ParseURI(link.data(), link.size())
    { }


    Protocol::~Protocol(void) = default;

}  // namespace protocols.
