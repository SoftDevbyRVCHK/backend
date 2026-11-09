#ifndef ODYSSEY_UTILITY_ALLOCATOR_FREELIST_HEADER_T_H
#define ODYSSEY_UTILITY_ALLOCATOR_FREELIST_HEADER_T_H
    
    struct header_t {
        
        header_t () : _size(0), _flags(0) {}
        
        void init (void *chunk_start) {
            std::memcpy(this, chunk_start, sizeof(header_t));
        }
        void copy (void *chunk_start) {
            std::memcpy(chunk_start, this, sizeof(header_t));
        }
        std::size_t size () {
            return  static_cast<std::size_t>(_size);
        }
        void size_set (std::size_t newsize) {
            _size = static_cast<size_mt>(newsize);
        }
        
        bool cuse () {
            return _flags & flags::cuse;
        }
        void cuse_0 () {
            _flags&= (~flags::cuse);
        }
        void cuse_1 () {
            _flags|= flags::cuse;
        }
        
        bool puse () {
            return _flags & flags::puse;
        }
        void puse_0 () {
            _flags&= (~flags::puse);
        }
        void puse_1 () {
            _flags|= flags::puse;
        }
        
        struct flags {
            
            static constexpr flags_t nuse {0};
            static constexpr flags_t cuse {1 << 0};
            static constexpr flags_t puse {1 << 1};
        };
        
    private:
        
        size_mt _size;
        flags_t _flags;
    };

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELIST_HEADER_T_H */
