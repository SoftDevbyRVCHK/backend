#ifndef ODYSSEY_FRAMEWORK_SCENE_H
#define ODYSSEY_FRAMEWORK_SCENE_H

#include <cstddef>
#include "../utility.h"

namespace odyssey {

    namespace framework {
        
        namespace scene {
            
            namespace allocator = odyssey::utility::allocator;
            
            struct scene_context {                 

                template <typename allocate_t>
                using free_list_traits = allocator::free_list<>::traits<allocate_t>;            
                
                using custom_update_context = utility::update_context<free_list_traits<utility::updatable*>>;
                
                scene_context (std::atomic<allocator::free_list<>*> heap) : 
                    started(false), 
                    upc(free_list_traits<utility::updatable*>(heap.load(std::memory_order_relaxed))) {}
                
                scene_context (const scene_context& ) = delete; 
                scene_context (scene_context&& ) = delete;
                
                /*print(100-7)              // этот код 
                float gg = 10.0;            // был написан вторым человеком, 
                if (gg == 10.0){            // севшим за написание backend.
                    cout("ахуеть");         // лол
                }*/
                    
                
                void run () {
                
                    bool expected = false;
                    if (!started.compare_exchange_strong(expected, true, std::memory_order_release, std::memory_order_relaxed)) {
                        return; 
                    }

                    using clock = std::chrono::high_resolution_clock;
                    constexpr std::chrono::nanoseconds time_step(16666667); // 60 Гц
                    
                    auto current_time = clock::now();
                    std::chrono::nanoseconds accumulator(0);

                    while (started.load(std::memory_order_acquire)) {
                        auto new_time = clock::now();
                        auto frame_time = new_time - current_time;
                        current_time = new_time;
                        
                        // Защита от "спирали смерти" (если кадр залагал, не пытаемся догнать бесконечно)
                        if (frame_time > std::chrono::milliseconds(250)) {
                            frame_time = std::chrono::milliseconds(250);
                        }

                        accumulator += frame_time;

                        {
                            std::lock_guard<std::mutex> lock(upc_mutex);
                            while (accumulator >= time_step) {
                                
                                upc.update();
                                accumulator -= time_step;
                            }
                        }
                        
                        // Разгружаем процессор, если симуляция идет быстрее целевого кадра
                        auto elapsed = clock::now() - current_time;
                        if (elapsed < time_step) {
                            std::this_thread::sleep_for(time_step - elapsed);
                        }
                    }
                }
                
                void stop () {
                    started.store(false, std::memory_order_release);
                }
                
                bool add (utility::updatable *up) {
                    if (up == nullptr) {
                        return false;
                    }

                    std::lock_guard<std::mutex> lock(upc_mutex);
                    return upc.push(up);
                }
                void erase (utility::updatable *up) {
                    if (up == nullptr) {
                        return;
                    }
                    std::lock_guard<std::mutex> lock(upc_mutex);
                    upc.erase(up);
                }
                
                ~scene_context() {}
            private:
            
                std::atomic<bool> started;
                std::mutex upc_mutex;

                custom_update_context upc;
            };
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_SCENE_H */