#pragma once

namespace ProxyQueue {
    // https://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
    struct Uri {
    public:
        std::string QueryString, Path, Protocol, Host, Port;

        static Uri Parse(const std::string &uri) {
            Uri result;

            //typedef std::string::const_iterator iterator_t;

            if (uri.length() == 0)
                return result;

            auto uriEnd = uri.end();

            // get query start
            auto queryStart = std::find(uri.begin(), uriEnd, '?');

            // protocol
            auto protocolStart = uri.begin();
            auto protocolEnd = std::find(protocolStart, uriEnd, ':');

            if (protocolEnd != uriEnd) {
                std::string prot = &*(protocolEnd);
                if ((prot.length() > 3) && (prot.substr(0, 3) == "://")) {
                    result.Protocol = std::string(protocolStart, protocolEnd);
                    protocolEnd += 3;
                } else
                    protocolEnd = uri.begin();// no protocol
            } else
                protocolEnd = uri.begin();// no protocol

            // host
            auto hostStart = protocolEnd;
            auto pathStart = std::find(hostStart, uriEnd, L'/');// get pathStart

            auto hostEnd = std::find(protocolEnd,
                                           (pathStart != uriEnd) ? pathStart : queryStart,
                                           ':');// check for port

            result.Host = std::string(hostStart, hostEnd);

            // port
            if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))// we have a port
            {
                hostEnd++;
                auto portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
                result.Port = std::string(hostEnd, portEnd);
            }

            // path
            if (pathStart != uriEnd)
                result.Path = std::string(pathStart, queryStart);

            // query
            if (queryStart != uriEnd)
                result.QueryString = std::string(queryStart, uri.end());
            return result;

        }// Parse
    };   // uri
}