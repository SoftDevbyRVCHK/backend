#ifndef ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H
#define ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H

#include <coroutine>
#include <utility>

namespace odyssey {

    namespace framework {

        namespace engine {

            template <typename interface_t>
            struct transformer {

                struct promise_type;

                using coro_t = std::coroutine_handle<promise_type>;

                struct promise_type {

                    promise_type () : interface(nullptr){}
                   ~promise_type () = default;

                    std::suspend_always initial_suspend () {return std::suspend_always();}
                    std::suspend_always final_suspend   () {return std::suspend_always();}
                    
                    void unhandled_exception () {throw;}

                    transformer get_return_object () {
                        return transformer(coro_t::from_promise(*this));
                    }

                    template <typename any_t>
                    auto await_transform (any_t &&) = delete;

                    std::suspend_never return_value (interface_t *object) {
                        
                        interface = object;

                        return std::suspend_never();
                    }

                    std::suspend_always yield_value (interface_t *object) {
                        
                        interface = object;
                        
                        return std::suspend_always();
                    }

                    interface_t *interface;
                };

                transformer (const transformer &other) = delete;
                transformer& operator= (const transformer &other) = delete;

                transformer (transformer&& other) noexcept
                    : handle(std::exchange(other.handle, {}))
                {}

                transformer& operator= (transformer&& other) noexcept {
                    if (this != &other) {
                        destroy();
                        handle = std::exchange(other.handle, {});
                    }

                    return *this;
                }

                bool operator== (const transformer &other) const {return handle == other.handle;}
                bool operator!= (const transformer &other) const {return handle != other.handle;}

                void transform () {

                    if (!handle.done()) {

                        handle.promise().interface = nullptr;

                        handle.resume();
                    }
                }

                interface_t* interface () {return handle.promise().interface;}

                ~transformer () {destroy();}

            private:
                coro_t handle;

                explicit transformer (coro_t handle) : handle(handle) {}
                
                void destroy () {
                    if (handle != nullptr) {
                        handle.destroy();
                        handle = nullptr;
                    }
                }
            };

        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H */