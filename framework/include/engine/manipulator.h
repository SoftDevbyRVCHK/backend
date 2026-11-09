#ifndef ODYSSEY_FRAMEWORK_ENGINE_MANIPULATOR_H
#define ODYSSEY_FRAMEWORK_ENGINE_MANIPULATOR_H

#include <concepts>
#include <memory>

namespace odyssey::framework::engine {

    template <typename T, typename interface_t>
    concept manipulatable = requires(T& value, interface_t *interface) {
        { value.manipulate(interface) } noexcept -> std::same_as<bool>;
    };

    template <typename interface_t>
    class manipulator {
    private:
        struct vtable_t {
            bool (*manipulate)(void*, interface_t*) noexcept;
        };

        template <typename T>
        static bool manipulate_impl(void* object, interface_t* interface) noexcept {
            return ((T*)object)->manipulate(interface);
        }

        template <typename T>
        static inline constexpr vtable_t vtable {
            &manipulate_impl<T>
        };

    public:
        template <typename T>
            requires manipulatable<T, interface_t>
                && (!std::same_as<T, manipulator<interface_t>>)
        explicit manipulator(T& object) noexcept
            : object((void*)std::addressof(object)),
            vptr(&vtable<T>)
        {}

        manipulator (const manipulator&) noexcept = default;

        manipulator& operator= (const manipulator&) noexcept = default;

        [[nodiscard]]
        bool manipulate (interface_t *interface) const noexcept {
            return vptr->manipulate(object, interface);
        }

    private:
        void* object;
        const vtable_t* vptr;
    };
}

#endif /* ODYSSEY_FRAMEWORK_ENGINE_MANIPULATOR_H */