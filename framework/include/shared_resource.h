#ifndef ODYSSEY_FRAMEWORK_SHAREDRESOURCE_H
#define ODYSSEY_FRAMEWORK_SHAREDRESOURCE_H

#include "preprocessor/core.h"

namespace odyssey::framework {

#define shared_resource_implement_method_BODY(base, method, ...)                \
    template <typename... Args>                                                 \
    requires requires (base_t& object, Args&&... args) {                        \
        object.method(std::forward<Args>(args)...);                             \
    }                                                                           \
    auto method(Args&&... args) {                                               \
        using result_t = decltype(                                              \
            std::declval<base_t&>().method(                                     \
                std::forward<Args>(args)...));                                  \
                                                                                \
        static_assert(                                                          \
            std::is_void_v<result_t> ||                                         \
            !std::is_reference_v<result_t>,                                     \
            "A synchronized wrapper cannot return references");                 \
                                                                                \
        std::lock_guard lock(mutex);                                            \
                                                                                \
        if constexpr (std::is_void_v<result_t>) {                               \
            base_t::method(std::forward<Args>(args)...);                        \
        } else {                                                                \
            return base_t::method(std::forward<Args>(args)...);                 \
        }                                                                       \
    }                                                                           \
                                                                                \
    template <typename... Args>                                                 \
    requires requires (const base_t& object, Args&&... args) {                  \
        object.method(std::forward<Args>(args)...);                             \
    }                                                                           \
    auto method(Args&&... args) const {                                         \
        using result_t = decltype(                                              \
            std::declval<const base_t&>().method(                               \
                std::forward<Args>(args)...));                                  \
                                                                                \
        static_assert(                                                          \
            std::is_void_v<result_t> ||                                         \
            !std::is_reference_v<result_t>,                                     \
            "A synchronized wrapper cannot return references");                 \
                                                                                \
        std::lock_guard lock(mutex);                                            \
                                                                                \
        if constexpr (std::is_void_v<result_t>) {                               \
            base_t::method(std::forward<Args>(args)...);                        \
        } else {                                                                \
            return base_t::method(std::forward<Args>(args)...);                 \
        }                                                                       \
    }                                                                           \
                                                                                \
    __VA_OPT__(FM_MKCALL(shared_resource_implement_method_BODY, base, __VA_ARGS__))

#define shared_resource(topic, base, method_tuple)                        \
    struct topic base {                                                   \
                                                                          \
        using base_t = base;                                              \
        using base_t::base_t;                                             \
                                                                          \
        shared_resource_implement_methods(base, TUPLE_TO_VA(method_tuple))\
                                                                          \
        template <typename invoker>                                       \
        decltype(auto) with_lock (invoker &&inv) {                        \
            std::lock_guard<std::mutex> lock(mutex);                      \
            return inv(static_cast<base_t&>(*this));                      \
        }                                                                 \
                                                                          \
        template <typename invoker>                                       \
        decltype(auto) with_lock (invoker &&inv) const {                  \
            std::lock_guard<std::mutex> lock(mutex);                      \
            return inv(static_cast<const base_t&>(*this));                \
        }                                                                 \
                                                                          \
    private:                                                              \
        mutable std::mutex mutex;                                         \
    }
}

#endif /* ODYSSEY_FRAMEWORK_SHAREDRESOURCE_H */