#include <session.h>

#include <string>
#include <utility>
#include <functional>
#include <memory>

#include <boost/asio.hpp>

namespace odyssey {
    
    namespace framework {
        
        namespace session { 

            using namespace boost;
            
            awaitable<system::error_code> arbitor_awaitable (
                tcp::acceptor &acceptor, 
                std::function<awaitable<void> (tcp::socket socket)> session_mng
            )
            {

                asio::any_io_executor executor {co_await asio::this_coro::executor};

                system::error_code ec;
                while (true) {

                    tcp::socket socket {
                        co_await acceptor.async_accept(asio::redirect_error(asio::use_awaitable, ec))
                    };
            
                    if (ec) {
                        // 1. Штатное завершение работы (сервер закрывают)
                        if (ec == asio::error::operation_aborted ) {
                            co_return ec; 
                        }
                        
                        // 2. Сетевой шум (ошибки клиентов и сбои протоколов Linux)
                        else
                        if ( ec == asio::error::connection_aborted  
                          || ec == asio::error::connection_reset   
                          || ec == asio::error::network_unreachable
                          || ec == asio::error::host_unreachable   
                          || ec == asio::error::no_buffer_space)
                        {
                            // Просто логируем (по желанию) и идем на следующий круг
                            continue;
                        }
                        
                        // 3. Временный дефицит ресурсов ОС (Закончились дескрипторы)
                        else 
                        if (ec == asio::error::no_descriptors) {
                            // Засыпаем на 100 мс, чтобы не утилизировать 100% процессора впустую
                            asio::steady_timer timer{executor, std::chrono::milliseconds(100)};
                            co_await timer.async_wait(asio::use_awaitable);
                            
                            continue;
                        }

                        // 4. Все остальные критические сбои (bad_descriptor, already_open и т.д.)
                        else {
                            co_return ec;
                        }                        
                    }
                    else
                        asio::co_spawn(
                            executor, 
                            session_mng(std::move(socket)), 
                            asio::detached
                        );
                }

                co_return ec;
            }
        }
    }
}