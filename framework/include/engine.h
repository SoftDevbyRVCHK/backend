#ifndef ODYSSEY_FRAMEWORK_ENGINE_H
#define ODYSSEY_FRAMEWORK_ENGINE_H

#include <coroutine>

/*
    template <>
    struct coroutine_handle<void> {

        address()
        from_address()
        done()
        resume()
        destroy()
    };

    template <typename promise_type>
    struct coroutine_handle : coroutine_handle<void> {

        promise()
        from_promise()
    };
*/

/*
    invoker corotine (...) {

        invoker::promise_type promise{};
        invoker retobj {promise.get_return_object()};

        co_await promise.initial_suspend();

        try { coro body }
        catch (...) {promise.unhandled_exception();}

    final_suspend:
        co_await promise.final_suspend();
    }
*/
namespace odyssey {

    namespace framework {

        namespace engine {

            template <typename interface_t>
            struct scheduler;

            template <typename interface_t>
            struct transformer;

            template <typename interface_t>
            struct manipulator;
        }
    }
}

#include "engine/sheduler.h"
#include "engine/transformer.h"
#include "engine/manipulator.h"

#endif /* ODYSSEY_FRAMEWORK_ENGINE_H */