#include <hv/HttpServer.h>
#include <hv/httpdef.h>
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

// ��������� ��� ������������
struct User {
    std::string id;
    std::string name;
    std::string email;
    int age;
    // �������� ������ ���� �� �������������
};

// ��������� ��� �������������
std::map<std::string, User> users;

int main() {
    hv::HttpServer server;
    server.POST("/user", [](const hv::HttpContextPtr& ctx) {
        try {
            // ������ JSON �� ���� �������
            auto jsonReq = json::parse(ctx->body());
            User user;
            user.id = jsonReq["id"];
            user.name = jsonReq["name"];
            user.email = jsonReq["email"];
            user.age = jsonReq["age"];
            // ��������� ������������ � ���������
            users[user.id] = user;

            // ���������� �����
            ctx->sendJson(json{ {"status", "ok"} });
            return 200;
        }
        catch (...) {
            // ��������� ����������
            ctx->response->status_code = HTTP_STATUS_BAD_REQUEST;
            return 400;
        }
        });

    server.GET("/users", [](const hv::HttpContextPtr& ctx) {
        // ������� JSON ������ �������������
        json jsonRes;
        for (const auto& [id, user] : users) {
            jsonRes.push_back({
                {"id", user.id},
                {"name", user.name},
                {"email", user.email},
                {"age", user.age}
                });
        }

        // ���������� �����
        ctx->sendJson(jsonRes);
        return 200;
        });

    server.DEL("/user/:id", [](const hv::HttpContextPtr& ctx) {
        // �������� ID ������������ �� ���� �������
        std::string userId = ctx->param("id");

        // ������� ������������ �� ���������
        if (users.erase(userId) > 0) {
            // ���������� ������������� �� ��������
            ctx->sendJson(json{ {"status", "deleted"} });
            return 200;
        }
        else {
            // ������������ �� ������
            ctx->sendJson(json{ {"status", "not found"} });
            return 404;
        }
        });

    // ��������� ������ �� ����� 8080
    if (server.start("0.0.0.0", 8080) != 0) {
        fprintf(stderr, "Can't start server\n");
        return -1;
    }

    return 0;
}
