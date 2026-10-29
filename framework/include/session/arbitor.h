#ifndef ODYSSEY_FRAMEWORK_SESSION_ARBITOR_H
#define ODYSSEY_FRAMEWORK_SESSION_ARBITOR_H

#include <string>
#include <utility>
#include <functional>
#include <memory>

#include <boost/asio.hpp>

namespace odyssey {
    
    namespace framework {
        
        namespace session {

            template <typename T>
            using awaitable = boost::asio::awaitable<T>;
            using tcp = boost::asio::ip::tcp;

            awaitable<boost::system::error_code> arbitor_awaitable (
                tcp::acceptor &acceptor, 
                std::function<awaitable<void> (tcp::socket)> session_mng
            );
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_SESSION_ARBITOR_H */