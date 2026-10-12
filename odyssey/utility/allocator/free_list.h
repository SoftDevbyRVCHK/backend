#ifndef ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H
#define ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H


namespace odyssey {

    namespace utility {
        
        namespace allocator {
            
            template <typename size_mt, typename flags_t>
            struct free_list_header {

                void set (size_mt newsize, flags_t newflags) {
                    _size  = newsize;
                    _flags = newflags;
                }
                    
                size_mt size     ()                 {return _size;}
                void    size_set (size_mt newsize)  {_size = newsize;}
                
                flags_t cuse     ()                 {return _flags;}
                void    cuse_set (flags_t newflags) {_flags^= (newflags & flags::cuse);}
                
                flags_t puse     ()                 {return _flags;}
                void    puse_set (flags_t newflags) {_flags^= (newflags & flags::puse);}
                
                struct flags {
                    
                    static constexpr flags_t nuse {0};
                    static constexpr flags_t cuse {1 << 0};
                    static constexpr flags_t puse {1 << 1};
                };
                
            private:
                
                size_mt _size;
                flags_t _flags;
            };
            
            template <
                typename header_t = free_list_header<std::size_t, std::uint8_t>,
                typename offset_t = std::uint8_t
            >
            struct free_list {
                
                struct ptrnext_t {
                    
                    ptrnext_t *next;
                };
                
                free_list (std::span<std::byte> chunk) : storage(chunk) {
                    
                    //soon
                }
                
               ~free_list () {
                    storage = std::span<std::byte>(); 
                    head = nullptr;
                }

                template <typename T>
                T* allocate () { 
                    return reinterpret_cast<T*>(
                        allocate(1 * sizeof(T), alignof(T))
                    );
                }
                template <typename T>
                T* allocate (std::size_t n) { 
                    return reinterpret_cast<T*>(
                        allocate(n * sizeof(T), alignof(T))
                    ); 
                }
                void *allocate (std::size_t sizeb, std::size_t alignb) {
                    
                    //soon
                }

                void deallocate (void *ptr) {
                    //soon
                }
                
                void clear () {
                    //soon
                }
                                                                
                free_list (const free_list &) = delete;
                free_list& operator= (const free_list &) = delete;
                
                free_list (free_list &&) = delete;
                free_list& operator= (free_list &&) = delete;
                
            protected:
                
                std::span<std::byte> storage;
                ptrnext_t *head;
                
                void* prepare (std::span<std::byte> chunk, std::size_t sizeb, std::size_t alignb) {
                    
                    std::byte* chunk_start {chunk.data()};
                    
                    chunk = chunk.subspan(sizeof(header_t) + sizeof(offset_t));
                    
                    if (!align(alignb, sizeb, chunk))
                        return nullptr;
                    
                    std::ptrdiff_t head_to_data = chunk.data() - chunk_start;
                    if (head_to_data > std::numeric_limits<offset_t>::max())
                        return nullptr;
                    
                    offset_t offset {static_cast<offset_t>(head_to_data)};
                    std::memcpy(
                        chunk.data() - sizeof(offset_t),
                        &offset,
                        sizeof(offset_t)
                    );
                    
                    return reinterpret_cast<void*>(chunk.data());
                }
                
                header_t* get_meta (void *ptr) {
                    
                    offset_t offset;
                    std::memcpy(
                        &offset,
                        static_cast<std::byte*>(ptr) - sizeof(offset_t),
                        sizeof(offset_t)
                    );
                    
                    return reinterpret_cast<header_t*>(static_cast<std::byte*>(ptr) - offset);
                }
                
                ptrnext_t* at_chunk (ptrnext_t *prev, std::span<std::byte> chunk) {
                    
                    std::byte *ptr {prepare(chunk, sizeof(ptrnext_t), alignof(ptrnext_t))};
                    
                    if (ptr == nullptr) 
                        return nullptr;
                    
                    if (chunk.size() < ptr - chunk.data() + sizeof(std::byte*))
                        return nullptr;
 
                    ptrnext_t *current {reinterpret_cast<ptrnext_t*>(ptr)};
                    
                    current->next = prev->next;
                    prev->next = current;
                    
                    header_t header;
                    
                    std::memcpy(&header, chunk.data(), sizeof(header_t));
                    header.set(
                        static_cast<decltype(header.size())>(chunk.size()),
                        header.puse()
                    );
                    std::memcpy(chunk.data(), &header, sizeof(header_t));
                    
                    
                    std::memcpy(chunk.data() + chunk.size() - sizeof(std::byte*), &ptr, sizeof(std::byte*));
                    
                    return current;
                }
            };
        }
    }
}

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELIST_H */