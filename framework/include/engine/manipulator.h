#ifndef ODYSSEY_FRAMEWORK_ENGINE_MANIPULATOR_H
#define ODYSSEY_FRAMEWORK_ENGINE_MANIPULATOR_H

#include <coroutine>
#include <utility>

namespace odyssey {

    namespace framework {

        namespace engine {

            template <typename interface_t>
            struct manipulator {

                struct promise_type;

                using coro_t = std::coroutine_handle<promise_type>;

                struct promise_type {
                    
                    promise_type () : interface(nullptr){}
                   ~promise_type () = default;

                    std::suspend_never  initial_suspend () {return std::suspend_never();}
                    std::suspend_always final_suspend   () {return std::suspend_always();}
                    
                    void unhandled_exception () {throw;}

                    manipulator get_return_object () {return manipulator(coro_t::from_promise(*this));}

                    template <typename any_t>
                    auto await_transform (any_t &&) = delete;

                    struct awaiter {
                        
                        awaiter (promise_type &promise) : promise(promise){}
                       ~awaiter () {promise.interface = nullptr;}

                        bool await_ready () {return false;}
                        
                        void await_suspend (coro_t) {}

                        interface_t* await_resume () {return promise.interface;}

                        promise_type& promise;
                    };
                    
                    std::suspend_never return_value (bool object) {

                        result = object;

                        return std::suspend_never();
                    }
                    awaiter yield_value (bool object) {
                        
                        result = object;
                        
                        return awaiter(*this);
                    }

                    interface_t *interface;
                    bool result;
                };

                manipulator (const manipulator &other) = delete;
                manipulator& operator= (const manipulator &other) = delete;
                
                manipulator (manipulator &&other) noexcept
                    : handle(std::exchange(other.handle, nullptr))
                {}

                manipulator& operator= (manipulator &&other) noexcept {
                    if (this != &other) {

                        destroy();
                        handle = std::exchange(other.handle, nullptr);
                    }

                    return *this;
                }

                bool operator== (const manipulator &other) const {return handle == other.handle;}
                bool operator!= (const manipulator &other) const {return handle != other.handle;}

                bool manipulate (interface_t *interface) {
                    if (!handle.done()) {
                        
                        handle.promise().interface = interface;
                    
                        handle.resume();

                        return handle.promise().result;
                    }
                    else
                        return false;
                }

                ~manipulator () {destroy();}

            private:
                coro_t handle;

                explicit manipulator (coro_t handle) : handle(handle) {}

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

#endif /* ODYSSEY_FRAMEWORK_ENGINE_MANIPULATOR_H */