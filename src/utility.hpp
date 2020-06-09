
namespace ProxyQueue {
    inline std::string CreateHash(const std::string &host, const std::string &body) {
        return std::to_string(std::hash<std::string>{}(host + body));
    };
}// namespace ProxyQueue
