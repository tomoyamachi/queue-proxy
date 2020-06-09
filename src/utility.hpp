
namespace ProxyQueue {
    inline std::string createHash(std::string host, std::string body) {
        return std::to_string(std::hash<std::string>{}(host + body));
    };
}// namespace ProxyQueue
