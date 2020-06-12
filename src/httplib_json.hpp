#pragma once
#include <nlohmann/json.hpp>
#include <httplib.h>

namespace httplib {
    inline void to_json(nlohmann::json &j, const Request &p) {
        j = nlohmann::json{
                {"method", p.method},
                {"target", p.target},
                {"path", p.path},
                {"body", p.body},
                {"content_length", p.content_length},
                {"remote_addr", p.remote_addr},
                {"headers", p.headers},
                {"params", p.params}};
    }
    inline void from_json(const nlohmann::json &j, Request &p) {
        j.at("method").get_to(p.method);
        j.at("target").get_to(p.target);
        j.at("path").get_to(p.path);
        j.at("body").get_to(p.body);
        j.at("content_length").get_to(p.content_length);
        j.at("remote_addr").get_to(p.remote_addr);
        j.at("headers").get_to(p.headers);
        j.at("params").get_to(p.params);
    }

    inline void to_json(nlohmann::json &j, const Response &p) {
        j = nlohmann::json{
                {"status", p.status},
                {"headers", p.headers},
                {"body", p.body},
                {"content_length", p.content_length_}};
    }

    inline void from_json(const nlohmann::json &j, Response &p) {
        j.at("status").get_to(p.status);
        j.at("body").get_to(p.body);
        j.at("headers").get_to(p.headers);
        j.at("content_length").get_to(p.content_length_);
    }
}// namespace httplib