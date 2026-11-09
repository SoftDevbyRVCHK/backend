#ifndef ODYSSEY_FRAMEWORK_ENGINE_SCHEDULERDETAILS_H
#define ODYSSEY_FRAMEWORK_ENGINE_SCHEDULERDETAILS_H

#include <tuple>
#include <vector>
#include <cstdint>
#include <memory>

namespace odyssey::framework::engine {

    template <
        typename job_t, 
        template <typename T> typename allocator_t = std::allocator
    >
    using job_base = std::vector<
        job_t,
        allocator_t<job_t>
    >;

    template <
        typename job_t, 
        template <typename T> typename allocator_t = std::allocator
    >
    struct job_store {

        trans_store () = default;
        ~trans_store () = default;
        
        trans_store (const trans_store& store) = default;
        trans_store& operator= (const trans_store& store) = default;
        
        trans_store (trans_store&& store) = default;
        trans_store& operator= (trans_store&& store) = default;

        struct iterator;

        template <typename T>
        bool create (T& object, handler_t& handler) {
            try {
                this->emplace_back(object);
                return true;
            }
            catch(...) {
                return false;
            }
        }


    private:
        job_base<
            job_t,
            allocator<job_t>
        > transforms;
    };
}

#endif /* ODYSSEY_FRAMEWORK_ENGINE_SCHEDULERDETAILS_H */