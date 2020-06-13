#pragma once
#include <httplib.h>
#include <string>

namespace ProxyQueue {
    void CreateDirectory(const std::string &path);
    void CreateDefaultDirectory();
    std::optional<httplib::Response> LoadResponse(const std::string &filename);
    std::string CreateFileUniqueID(const httplib::Request &req);
    bool SaveAsRequestQueue(const std::string &requestFile, const httplib::Request &req);
}// namespace ProxyQueue