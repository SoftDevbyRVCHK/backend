#ifndef ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H
#define ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H

#include <concepts>
#include <memory>

namespace odyssey::framework::engine {

    template <typename T, typename interface_t>
    concept transformable = requires(T& value) {
        { value.transform() } noexcept -> std::same_as<void>;

        { value.interface() } noexcept
            -> std::convertible_to<interface_t*>;

        { static_cast<interface_t*>(value.interface()) } noexcept
            -> std::same_as<interface_t*>;
    };

    template <typename interface_t>
    class transformer {
    private:
        struct vtable_t {
            void (*transform)(void*) noexcept;
            interface_t* (*interface)(void*) noexcept;
        };

        template <typename T>
        static void transform_impl(void* object) noexcept {
            ((T*)object)->transform();
        }

        template <typename T>
        static interface_t* interface_impl(void* object) noexcept {
            return static_cast<interface_t*>(
                ((T*)object)->interface()
            );
        }

        template <typename T>
        static inline constexpr vtable_t vtable {
            &transform_impl<T>,
            &interface_impl<T>
        };

    public:
        template <typename T>
            requires transformable<T, interface_t>
                && (!std::same_as<T, transformer<interface_t>>)
        explicit transformer(T& object) noexcept
            : object((void*)std::addressof(object)),
            vptr(&vtable<T>)
        {}

        transformer (const transformer&) noexcept = default;

        transformer& operator= (const transformer&) noexcept = default;

        void transform () const noexcept {
            vptr->transform(object);
        }

        [[nodiscard]]
        interface_t* interface () const noexcept {
            return vptr->interface(object);
        }

    private:
        void* object;
        const vtable_t* vptr;
    };
}

#endif /* ODYSSEY_FRAMEWORK_ENGINE_TRANSFORMER_H */