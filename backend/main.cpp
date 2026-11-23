#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include "../oop/ResponseSystem.h"

#pragma comment(lib, "Ws2_32.lib")

static ResponseSystem systemCore;

static std::string jsonEscape(const std::string& s) {
    std::string out;
    for (unsigned char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: if (c >= 32) out += (char)c;
        }
    }
    return out;
}

static std::string getJsonString(const std::string& body, const std::string& key) {
    std::string needle = "\"" + key + "\"";
    size_t p = body.find(needle);
    if (p == std::string::npos) return "";
    p = body.find(':', p + needle.size());
    if (p == std::string::npos) return "";
    p = body.find('"', p + 1);
    if (p == std::string::npos) return "";
    size_t end = body.find('"', p + 1);
    if (end == std::string::npos) return "";
    return body.substr(p + 1, end - p - 1);
}

static int getJsonInt(const std::string& body, const std::string& key, int fallback) {
    std::string needle = "\"" + key + "\"";
    size_t p = body.find(needle);
    if (p == std::string::npos) return fallback;
    p = body.find(':', p + needle.size());
    if (p == std::string::npos) return fallback;
    ++p;
    while (p < body.size() && std::isspace((unsigned char)body[p])) ++p;
    try { return std::stoi(body.substr(p)); } catch (...) { return fallback; }
}

static std::string emergenciesJson() {
    auto list = systemCore.getEmergencies();
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < list.size(); ++i) {
        const auto& e = list[i];
        if (i) out << ",";
        out << "{\"id\":" << e.id
            << ",\"type\":\"" << jsonEscape(e.type)
            << "\",\"location\":\"" << jsonEscape(e.location)
            << "\",\"severity\":" << e.severity
            << ",\"severityName\":\"" << getSeverityName(e.severity)
            << "\",\"description\":\"" << jsonEscape(e.description)
            << "\",\"status\":\"" << jsonEscape(e.status) << "\"}";
    }
    out << "]";
    return out.str();
}

static std::string makeResponse(int status, const std::string& statusText,
                                const std::string& contentType,
                                const std::string& body) {
    std::ostringstream out;
    out << "HTTP/1.1 " << status << " " << statusText << "\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Access-Control-Allow-Origin: *\r\n"
        << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        << "Access-Control-Allow-Headers: Content-Type\r\n"
        << "Connection: close\r\n\r\n" << body;
    return out.str();
}

static std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    std::ostringstream ss; ss << file.rdbuf(); return ss.str();
}

static std::string mimeType(const std::string& path) {
    if (path.size() >= 5 && path.substr(path.size()-5) == ".html") return "text/html; charset=utf-8";
    if (path.size() >= 4 && path.substr(path.size()-4) == ".css") return "text/css; charset=utf-8";
    if (path.size() >= 3 && path.substr(path.size()-3) == ".js") return "application/javascript; charset=utf-8";
    return "text/plain; charset=utf-8";
}

static std::string handleRequest(const std::string& method,
                                 const std::string& target,
                                 const std::string& body) {
    if (method == "OPTIONS") return makeResponse(204, "No Content", "text/plain", "");
    if (method == "GET" && target == "/api/health")
        return makeResponse(200, "OK", "application/json", "{\"ok\":true,\"service\":\"LifeLine C++ backend\"}");
    if (method == "GET" && target == "/api/emergencies")
        return makeResponse(200, "OK", "application/json", emergenciesJson());
    if (method == "GET" && target == "/api/resources")
        return makeResponse(200, "OK", "application/json", systemCore.getResourcesJson());

    if (method == "POST" && target == "/api/emergencies") {
        std::string type = getJsonString(body, "type");
        std::string location = getJsonString(body, "location");
        std::string description = getJsonString(body, "description");
        int severity = getJsonInt(body, "severity", 0), id = 0;
        std::string error;
        if (!systemCore.addEmergency(type, location, severity, description, id, error)) {
            return makeResponse(400, "Bad Request", "application/json",
                "{\"success\":false,\"message\":\"" + jsonEscape(error) + "\"}");
        }
        return makeResponse(201, "Created", "application/json",
            "{\"success\":true,\"id\":" + std::to_string(id) + ",\"message\":\"Emergency registered.\"}");
    }

    if (method == "POST" && target.rfind("/api/assign/", 0) == 0) {
        int id = 0;
        try { id = std::stoi(target.substr(std::string("/api/assign/").size())); } catch (...) {}
        ResponseResult r = systemCore.assignEmergency(id);
        std::ostringstream out;
        out << "{\"success\":" << (r.success ? "true" : "false")
            << ",\"message\":\"" << jsonEscape(r.message) << "\""
            << ",\"resourceId\":" << r.resourceId
            << ",\"resourceName\":\"" << jsonEscape(r.resourceName) << "\""
            << ",\"hospitalName\":\"" << jsonEscape(r.hospitalName) << "\""
            << ",\"distanceKm\":" << r.distanceKm
            << ",\"route\":\"" << jsonEscape(r.route) << "\"}";
        return makeResponse(r.success ? 200 : 409, r.success ? "OK" : "Conflict",
                            "application/json", out.str());
    }

    if (method == "GET") {
        std::string path = target;
        if (path == "/") path = "/index.html";
        if (path.find("..") != std::string::npos)
            return makeResponse(400, "Bad Request", "text/plain", "Invalid path");
        std::string content = readFile("frontend" + path);
        if (content.empty())
            return makeResponse(404, "Not Found", "text/plain", "File not found. Run server from project root.");
        return makeResponse(200, "OK", mimeType(path), content);
    }
    return makeResponse(404, "Not Found", "application/json", "{\"error\":\"Endpoint not found\"}");
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed.\n"; return 1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        std::cerr << "Could not create server socket.\n";
        WSACleanup(); return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(8080);

    if (bind(server, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR ||
        listen(server, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Could not bind to port 8080. Is another server using it?\n";
        closesocket(server); WSACleanup(); return 1;
    }

    std::cout << "LifeLine C++ backend running at http://localhost:8080\n";
    std::cout << "Keep this terminal open. Press Ctrl+C to stop.\n";

    while (true) {
        SOCKET client = accept(server, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        std::string request;
        char buffer[4096];
        int received;
        size_t headerEnd = std::string::npos;
        int contentLength = 0;

        do {
            received = recv(client, buffer, sizeof(buffer), 0);
            if (received > 0) {
                request.append(buffer, received);
                headerEnd = request.find("\r\n\r\n");
                if (headerEnd != std::string::npos) {
                    std::string headers = request.substr(0, headerEnd);
                    size_t cp = headers.find("Content-Length:");
                    if (cp != std::string::npos) {
                        cp += 15;
                        while (cp < headers.size() && headers[cp] == ' ') ++cp;
                        try { contentLength = std::stoi(headers.substr(cp)); } catch (...) {}
                    }
                }
            }
        } while (received > 0 &&
                 (headerEnd == std::string::npos ||
                  request.size() < headerEnd + 4 + (size_t)contentLength));

        std::string method, target, body;
        if (headerEnd != std::string::npos) {
            std::istringstream firstLine(request.substr(0, request.find("\r\n")));
            firstLine >> method >> target;
            body = request.substr(headerEnd + 4);
        }
        std::string response = handleRequest(method, target, body);
        send(client, response.c_str(), (int)response.size(), 0);
        shutdown(client, SD_SEND);
        closesocket(client);
    }

    closesocket(server);
    WSACleanup();
    return 0;
}
