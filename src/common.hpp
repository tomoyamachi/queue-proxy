#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <algorithm>// find
#include <chrono>
#include <cstdlib>// stdlib(c言語)のc++ version
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace ProxyQueue {
    template<class T>
    inline T GetEnvOrDefault(const std::string &variable_name, const T &default_value) {
        if (const char *value = std::getenv(variable_name.c_str())) {
            std::istringstream is(value);
            T t;
            if (is >> t) {
                return t;
            }
        }
        return default_value;
    }

    const std::string PROXY_ADDRESS = GetEnvOrDefault<std::string>("PROXY_ADDRESS", "localhost");
    const int PROXY_PORT = GetEnvOrDefault("PROXY_PORT", 1234);
    const std::string ROOT_DIR = GetEnvOrDefault<std::string>("ROOT_DIR", "tmp");
    const std::string QUEUE_DIR = ROOT_DIR + "/queue";
    const std::string QUEUE_REQ_DIR = QUEUE_DIR + "/req";
    const std::string QUEUE_RES_DIR = QUEUE_DIR + "/response";
    const std::string CONNECTION_PATH = ROOT_DIR + "/connection.json";
    const int CONNECTION_DURATION = GetEnvOrDefault("CONNECTION_DURATION", 10000);
    const int REQUEST_DURATION = GetEnvOrDefault("REQUEST_DURATION", 1000);

    const std::string METHOD_POST = "POST";
    const std::string METHOD_GET = "GET";
    const std::string METHOD_PUT = "PUT";
    const std::string METHOD_DELETE = "DELETE";

    // 307 Temporary Redirect https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/307
    const int STATUS_TEMPRARY_REDIRECT = 307;

    inline std::string CreateHash(const std::string &unique) {
        return std::to_string(std::hash<std::string>{}(unique));
    };


    // https://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform
    struct Uri {
    public:
        std::string QueryString, Path, Protocol, Host, Port;

        static Uri Parse(const std::string &uri) {
            Uri result;

            typedef std::string::const_iterator iterator_t;

            if (uri.length() == 0)
                return result;

            iterator_t uriEnd = uri.end();

            // get query start
            iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

            // protocol
            iterator_t protocolStart = uri.begin();
            iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');

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
            iterator_t hostStart = protocolEnd;
            iterator_t pathStart = std::find(hostStart, uriEnd, L'/');// get pathStart

            iterator_t hostEnd = std::find(protocolEnd,
                                           (pathStart != uriEnd) ? pathStart : queryStart,
                                           ':');// check for port

            result.Host = std::string(hostStart, hostEnd);

            // port
            if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))// we have a port
            {
                hostEnd++;
                iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
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
}// namespace ProxyQueue