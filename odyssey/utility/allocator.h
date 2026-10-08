#ifndef ODYSSEY_UTILITY_ALLOCATOR_H
#define ODYSSEY_UTILITY_ALLOCATOR_H

#include <cstddef>
#include <limits>
#include <cstring>
#include <span>
#include <new>
#include <memory>

namespace odyssey {

    namespace utility {
        
        namespace allocator {
            
            inline bool align(
                std::size_t alignment,
                std::size_t size,
                std::span<std::byte> &memory
            ) noexcept {
                if (alignment == 0 || (alignment & (alignment - 1)) != 0)
                    return false;

                void* ptr = memory.data();
                std::size_t space = memory.size();

                if (std::align(alignment, size, ptr, space) == nullptr)
                    return false;

                memory = std::span<std::byte>(static_cast<std::byte*>(ptr), space);

                return true;
            }
        }
    }
}

#include "allocator/stack_allocator.h"
#include "allocator/free_list_allocator.h"

#endif /* ODYSSEY_UTILITY_ALLOCATOR_H */