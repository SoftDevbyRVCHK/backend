#ifndef ODYSSEY_UTILITY_SERIALIZATION_H
#define ODYSSEY_UTILITY_SERIALIZATION_H

#include <cstdint>
#include <span>

namespace odyssey {

    namespace utility {
                
        struct serializable {
            
            using srlzn_t = std::uint16_t;
            using state_t = std::uint16_t;
            
            struct srlzn_flags {
                static constexpr srlzn_t message = 0;
                static constexpr srlzn_t state   = 1 << 0;
                static constexpr srlzn_t meta    = 1 << 1;
                static constexpr srlzn_t storage = 1 << 2;
                
                static constexpr srlzn_t json = 1 << 14;
                static constexpr srlzn_t bin  = 1 << 15;
            };
            
            struct state_flags {
                static constexpr state_t good    = 0;
                static constexpr state_t repeat  = 1 << 0;
                static constexpr state_t eodata  = 1 << 1;
            };

            virtual state_t write (std::span<std::uint8_t> out_buf, srlzn_t flags) const = 0;

            virtual state_t read  (std::span<const std::uint8_t> in_buf, srlzn_t flags) = 0;

            virtual std::size_t sizeb (srlzn_t flags) const = 0;
            
            virtual ~serializable() = default; 
        };
    }
}

#endif /* ODYSSEY_UTILITY_SERIALIZATION_H */