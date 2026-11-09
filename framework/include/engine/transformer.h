#ifndef ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H
#define ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H

#include <coroutine>
#include <utility>
#include <cstdint>
#include <tuple>
#include <memory>
#include <iterator>

namespace odyssey::framework::engine {

    /*template <
        typename interface_t
    >
    struct transformer {

        struct promise_type;

        using coro_t = std::coroutine_handle<promise_type>;

        struct promise_type {

            promise_type () : interface(nullptr){}
           ~promise_type () = default;

            template <typename any_t>
            auto await_transform (any_t &&) = delete;

            transformer get_return_object () {return transformer(coro_t::from_promise(*this));}

            std::suspend_always initial_suspend () {return std::suspend_always();}
            std::suspend_always final_suspend   () {return std::suspend_always();}
            
            void unhandled_exception () {throw;}

            std::suspend_always yield_value (interface_t *object) {
                
                interface = object;
                
                return std::suspend_always();
            }

            void return_value (interface_t *object) {interface = object;}

            interface_t *interface;
        };

        transformer (const transformer &other) = delete;
        transformer& operator= (const transformer &other) = delete;

        transformer (transformer&& other) : handle(std::exchange(other.handle, {})){}

        transformer& operator= (transformer&& other) {
            if (this != &other) {
                destroy();
                handle = std::exchange(other.handle, {});
            }

            return *this;
        }

        bool operator== (const transformer &other) const {return handle == other.handle;}
        bool operator!= (const transformer &other) const {return !this->operator==(other);}

        void transform () {

            if (!handle.done()) {

                handle.promise().interface = nullptr;

                handle.resume();
            }
        }

        interface_t* interface () {return handle.promise().interface;}

        ~transformer () {destroy();}

    protected:
        coro_t handle;
        
        transformer (coro_t handle) : handle(handle) {}

        void destroy () {
            if (handle != nullptr) {
                handle.destroy();
                handle = nullptr;
            }
        }
    };
    
    template <
        typename interface_t,
        typename allocator_t = std::allocator<transformer<interface_t>>
    >
    struct transformator 
        : public std::list<
                transformer<interface_t>, 
                allocator_t
            >
    {
    };*/

    template <typename T, typename interface_t>
    concept transformable = requires(T a) {
        { a.transform() } -> std::same_as<void>;
        { a.interface() } -> std::same_as<interface_t*>;
    };

    template <typename interface_t>
    struct transformer {

        template <typename T>
        requires transformable<T, interface_t>
        transformer(T *obj) 
            : obj(obj), 
            _transform(&obj_transform<T>),
            _interface(&obj_interface<T>)
        {}

        void transform() {_transform(obj);}
        
        interface_t* interface() {return _interface(obj);}

    private:

        void* obj;

        void         (*_transform) (void*);
        interface_t* (*_interface) (void*);

        template <typename T>
        static void obj_transform (void* obj) {
            static_cast<T*>(obj)->transform();
        }

        template <typename T>
        static interface_t* obj_interface (void* obj) {
            return static_cast<T*>(obj)->interface();
        }
    };
}

#endif /* ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H */