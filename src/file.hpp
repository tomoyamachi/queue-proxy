#pragma once

namespace ProxyQueue {
    void CreateDirectory(const std::string &path) {
        if (std::filesystem::create_directories(path)) {
            spdlog::info("create {}", path);
        } else {
            spdlog::error("failed create {}", path);
        }
    }

    void CreateDefaultDirectory() {
        CreateDirectory(QUEUE_REQ_DIR);
        CreateDirectory(QUEUE_RES_DIR);
    }

    std::optional<httplib::Response> LoadResponse(const std::string &filename) {
        std::ifstream resfile(filename);
        if (!resfile) {
            return std::nullopt;
        }
        //std::cout << resfile.rdbuf() << std::endl;
        httplib::Response p = nlohmann::json::parse(resfile);
        return p;
    }


    std::string CreateFileUniqueID(const httplib::Request &req) {
        // TODO: ヘッダー/REMOTE_PORTなどがuniqueにならない
        // filter fields
        httplib::Request req2;
        req2.method = req.method;
        req2.target = req.target;
        req2.body = req.body;

        // TODO : 他に方法がないか確認
        nlohmann::json j = req2;
        std::stringstream ss;
        ss << j;
        return CreateHash(ss.str());
    }


    bool SaveAsRequestQueue(const std::string &requestFile, const httplib::Request &req) {
        if (std::filesystem::exists(requestFile)) {
            return false;
        }
        nlohmann::json j = req;
        std::ofstream ofs(requestFile);
        ofs << j;
        return true;
    }
}// namespace ProxyQueue