#include <iostream>
#include <string>
#include <memory>

#include "header.h"

#include "boost/beast/core.hpp"
#include "boost/beast/websocket.hpp"
#include "boost/asio/ip/tcp.hpp"

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

asio::awaitable<void> clients_arbitr(unsigned short port) {
    // Получаем текущий исполнитель (executor) из контекста корутины
    auto executor = co_await asio::this_coro::executor;
    
    auto const address = asio::ip::make_address("0.0.0.0");
    tcp::acceptor acceptor{executor, {address, port}};
    
    std::cout << "Asynchronous Coroutine WebSocket server started on port " << port << "...\n";
    std::size_t next_client_id = 0;

    while (true) {
        // АСИНХРОННО ждем подключение. Поток свободен и обрабатывает других клиентов!
        tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
        std::cout << "New TCP connection accepted from: " << socket.remote_endpoint() << "\n";

        // Создаем контекст нового клиента в куче
        auto session = std::make_shared<client_context>(std::move(socket), next_client_id++);

        // Выстреливаем независимую корутину жизненного цикла клиента в планировщик
        asio::co_spawn(executor, 
                       [session]() { return session->start(); }, 
                       asio::detached);
    }
}

int main() {
    try {
        // Задаем io_context
        asio::io_context ioc{1};

        // Регистрируем корутину-слушатель на порту 8080
        asio::co_spawn(ioc, clients_arbitr(8080), asio::detached);

        // Запускаем планировщик событий. Теперь он будет крутиться здесь,
        // асинхронно переключаясь между listener и сессиями клиентов.
        ioc.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Global error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return 0;
}
