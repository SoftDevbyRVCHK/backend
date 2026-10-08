#ifndef ODYSSEY__UTILITY_SESSION_H
#define ODYSSEY__UTILITY_SESSION_H

#include <string>
#include <utility>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace odyssey {
    
    namespace utility {
    
        boost::asio::awaitable<boost::system::error_code> arbitor(
            boost::asio::ip::tcp::acceptor &acceptor, 
            void (*new_session) (boost::asio::ip::tcp::socket&&, std::size_t)
            )
        {

            boost::asio::any_io_executor executor {co_await boost::asio::this_coro::executor};

            std::size_t context_id {0};

            boost::system::error_code ec;
            while (true) {

                boost::asio::ip::tcp::socket socket {
                    co_await acceptor.async_accept(boost::asio::redirect_error(boost::asio::use_awaitable, ec))
                };
        
                if (ec) {
                    // 1. Штатное завершение работы (сервер закрывают)
                    if (ec == boost::asio::error::operation_aborted ) {
                        co_return ec; 
                    }
                    
                    // 2. Сетевой шум (ошибки клиентов и сбои протоколов Linux)
                    else
                    if ( ec == boost::asio::error::connection_aborted  
                      || ec == boost::asio::error::connection_reset   
                      || ec == boost::asio::error::network_unreachable
                      || ec == boost::asio::error::host_unreachable   
                      || ec == boost::asio::error::no_buffer_space)
                    {
                        // Просто логируем (по желанию) и идем на следующий круг
                        continue;
                    }
                    
                    // 3. Временный дефицит ресурсов ОС (Закончились дескрипторы)
                    else 
                    if (ec == boost::asio::error::no_descriptors) {
                        // Засыпаем на 100 мс, чтобы не утилизировать 100% процессора впустую
                        boost::asio::steady_timer timer{executor, std::chrono::milliseconds(100)};
                        co_await timer.async_wait(boost::asio::use_awaitable);
                        
                        continue;
                    }

                    // 4. Все остальные критические сбои (bad_descriptor, already_open и т.д.)
                    else {
                        co_return ec;
                    }                        
                }
                else
                    new_session(std::move(socket), context_id++);
            }

            co_return ec;
        }
    }
}

#endif /* ODYSSEY__UTILITY_SESSION_H */