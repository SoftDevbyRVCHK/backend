#ifndef ODYSSEY_UTILITY_ALLOCATOR_FREELIST_TRAITS_H
#define ODYSSEY_UTILITY_ALLOCATOR_FREELIST_TRAITS_H

template <
    typename T,
    typename SizeMt   = std::size_t, 
    typename FlagsT   = std::uint8_t,
    typename OffsetT  = std::uint16_t
>
class traits {
public:
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    using traits_type = free_list<SizeMt, FlagsT, OffsetT>;

    // Требуется для std::allocator_traits (rebind для работы с узлами нодовых контейнеров: list, map и т.д.)
    template <typename U>
    struct rebind {
        using other = traits<U, SizeMt, FlagsT, OffsetT>;
    };

    // Конструктор по умолчанию (пустой аллокатор)
    constexpr traits() noexcept 
        : pool_(nullptr) {}

    // Конструктор с привязкой к существующему free_list
    explicit traits(traits_type* pool) noexcept 
        : pool_(pool) {}

    // Копирующий конструктор
    traits(const traits& other) noexcept
        : pool_(other.pool_.load(std::memory_order_relaxed)) {}

    // Конструктор преобразования (rebind-конструктор)
    template <typename U>
    traits(const traits<U, SizeMt, FlagsT, OffsetT>& other) noexcept
        : pool_(other.pool_.load(std::memory_order_relaxed)) {}

    // Оператор присваивания
    traits& operator=(const traits& other) noexcept {
        if (this != &other) {
            pool_.store(other.pool_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }

    // Выделение памяти под n элементов типа T
    [[nodiscard]] T* allocate(std::size_t n) {
        if (n == 0) {
            return nullptr;
        }

        traits_type* current_pool = pool_.load(std::memory_order_acquire);
        if (!current_pool) {
            throw std::bad_alloc();
        }

        std::size_t bytes_to_allocate = n * sizeof(T);
        std::size_t alignment = alignof(T);

        // Синхронизация доступа к операциям со свободным списком
        void* ptr = nullptr;
        {
            static std::mutex alloc_mutex;
            std::lock_guard<std::mutex> lock(alloc_mutex);
            ptr = current_pool->allocate(bytes_to_allocate, alignment);
        }

        if (!ptr) {
            throw std::bad_alloc();
        }

        return static_cast<T*>(ptr);
    }

    // Освобождение памяти
    void deallocate(T* ptr, std::size_t /*n*/) noexcept {
        if (!ptr) {
            return;
        }

        traits_type* current_pool = pool_.load(std::memory_order_acquire);
        if (current_pool) {
            static std::mutex alloc_mutex;
            std::lock_guard<std::mutex> lock(alloc_mutex);
            current_pool->deallocate(ptr);
        }
    }

    // Смена активного пула на лету атомарно
    void set_pool(traits_type* new_pool) noexcept {
        pool_.store(new_pool, std::memory_order_release);
    }

    // Получение текущего пула
    traits_type* get_pool() const noexcept {
        return pool_.load(std::memory_order_acquire);
    }

    // Сравнение аллокаторов: аллокаторы равны, если ссылаются на один и тот же пул
    template <typename U>
    bool operator==(const traits<U, SizeMt, FlagsT, OffsetT>& other) const noexcept {
        return pool_.load(std::memory_order_relaxed) == other.pool_.load(std::memory_order_relaxed);
    }

    template <typename U>
    bool operator!=(const traits<U, SizeMt, FlagsT, OffsetT>& other) const noexcept {
        return !(*this == other);
    }

    template <typename, typename, typename, typename>
    friend class traits;

private:
    std::atomic<traits_type*> pool_{nullptr};
};

#endif /* ODYSSEY_UTILITY_ALLOCATOR_FREELIST_TRAITS_H */