#include <hv/HttpServer.h>
#include <hv/httpdef.h>
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

// Структура для пользователя
struct User {
    std::string id;
    std::string name;
    std::string email;
    int age;
    // Добавьте другие поля по необходимости
};

// Хранилище для пользователей
std::map<std::string, User> users;

int main() {
    hv::HttpServer server;
    server.POST("/user", [](const hv::HttpContextPtr& ctx) {
        try {
            // Парсим JSON из тела запроса
            auto jsonReq = json::parse(ctx->body());
            User user;
            user.id = jsonReq["id"];
            user.name = jsonReq["name"];
            user.email = jsonReq["email"];
            user.age = jsonReq["age"];
            // Добавляем пользователя в хранилище
            users[user.id] = user;

            // Отправляем ответ
            ctx->sendJson(json{ {"status", "ok"} });
            return 200;
        }
        catch (...) {
            // Обработка исключений
            ctx->response->status_code = HTTP_STATUS_BAD_REQUEST;
            return 400;
        }
        });

    server.GET("/users", [](const hv::HttpContextPtr& ctx) {
        // Создаем JSON массив пользователей
        json jsonRes;
        for (const auto& [id, user] : users) {
            jsonRes.push_back({
                {"id", user.id},
                {"name", user.name},
                {"email", user.email},
                {"age", user.age}
                });
        }

        // Отправляем ответ
        ctx->sendJson(jsonRes);
        return 200;
        });

    server.DEL("/user/:id", [](const hv::HttpContextPtr& ctx) {
        // Получаем ID пользователя из пути запроса
        std::string userId = ctx->param("id");

        // Удаляем пользователя из хранилища
        if (users.erase(userId) > 0) {
            // Отправляем подтверждение об удалении
            ctx->sendJson(json{ {"status", "deleted"} });
            return 200;
        }
        else {
            // Пользователь не найден
            ctx->sendJson(json{ {"status", "not found"} });
            return 404;
        }
        });

    // Запускаем сервер на порту 8080
    if (server.start("0.0.0.0", 8080) != 0) {
        fprintf(stderr, "Can't start server\n");
        return -1;
    }

    return 0;
}
