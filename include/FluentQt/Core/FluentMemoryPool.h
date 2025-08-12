// include/FluentQt/Core/FluentMemoryPool.h
#pragma once

#include <QMutex>
#include <QObject>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace FluentQt::Core {

// Memory pool for efficient allocation of small objects
template <typename T, size_t BlockSize = 4096>
class FluentMemoryPool {
public:
    static_assert(sizeof(T) >= sizeof(void*), "Type too small for memory pool");

    FluentMemoryPool() = default;
    ~FluentMemoryPool() { clear(); }

    // Non-copyable, non-movable
    FluentMemoryPool(const FluentMemoryPool&) = delete;
    FluentMemoryPool& operator=(const FluentMemoryPool&) = delete;
    FluentMemoryPool(FluentMemoryPool&&) = delete;
    FluentMemoryPool& operator=(FluentMemoryPool&&) = delete;

    // Allocate object
    T* allocate() {
        QMutexLocker locker(&m_mutex);

        if (m_freeList) {
            T* result = static_cast<T*>(m_freeList);
            m_freeList = *static_cast<void**>(m_freeList);
            ++m_allocatedCount;
            return result;
        }

        // Need new block
        if (m_currentBlock == nullptr || m_currentOffset >= BlockSize) {
            allocateNewBlock();
        }

        T* result = reinterpret_cast<T*>(m_currentBlock + m_currentOffset);
        m_currentOffset += sizeof(T);
        ++m_allocatedCount;
        return result;
    }

    // Deallocate object
    void deallocate(T* ptr) {
        if (!ptr)
            return;

        QMutexLocker locker(&m_mutex);

        // Add to free list
        *static_cast<void**>(ptr) = m_freeList;
        m_freeList = ptr;
        --m_allocatedCount;
    }

    // Construct object in place
    template <typename... Args>
    T* construct(Args&&... args) {
        T* ptr = allocate();
        try {
            new (ptr) T(std::forward<Args>(args)...);
            return ptr;
        } catch (...) {
            deallocate(ptr);
            throw;
        }
    }

    // Destroy and deallocate object
    void destroy(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr);
        }
    }

    // Statistics
    size_t allocatedCount() const {
        QMutexLocker locker(&m_mutex);
        return m_allocatedCount;
    }

    size_t totalBlocks() const {
        QMutexLocker locker(&m_mutex);
        return m_blocks.size();
    }

    size_t totalMemory() const {
        QMutexLocker locker(&m_mutex);
        return m_blocks.size() * BlockSize;
    }

    // Clear all memory (dangerous if objects still exist)
    void clear() {
        QMutexLocker locker(&m_mutex);
        for (char* block : m_blocks) {
            delete[] block;
        }
        m_blocks.clear();
        m_currentBlock = nullptr;
        m_currentOffset = BlockSize;
        m_freeList = nullptr;
        m_allocatedCount = 0;
    }

private:
    void allocateNewBlock() {
        char* newBlock = new char[BlockSize];
        m_blocks.push_back(newBlock);
        m_currentBlock = newBlock;
        m_currentOffset = 0;
    }

private:
    mutable QMutex m_mutex;
    std::vector<char*> m_blocks;
    char* m_currentBlock{nullptr};
    size_t m_currentOffset{BlockSize};
    void* m_freeList{nullptr};
    size_t m_allocatedCount{0};
};

// Global memory pool manager
class FluentMemoryManager : public QObject {
    Q_OBJECT

public:
    static FluentMemoryManager& instance();

    // Pool management
    template <typename T>
    FluentMemoryPool<T>& getPool() {
        static FluentMemoryPool<T> pool;
        return pool;
    }

    // Global allocation tracking
    void trackAllocation(size_t size, const QString& category = "Unknown");
    void trackDeallocation(size_t size, const QString& category = "Unknown");

    // Memory statistics
    struct MemoryStats {
        size_t totalAllocated{0};
        size_t totalDeallocated{0};
        size_t currentUsage{0};
        size_t peakUsage{0};
        std::unordered_map<QString, size_t> categoryUsage;
    };

    MemoryStats getStats() const;
    void resetStats();

    // Memory optimization
    void optimizeMemoryUsage();
    void enableMemoryTracking(bool enable = true);
    bool isMemoryTrackingEnabled() const { return m_trackingEnabled; }

    // Memory pressure handling
    void handleMemoryPressure();
    void setMemoryPressureThreshold(size_t threshold) {
        m_pressureThreshold = threshold;
    }

signals:
    void memoryPressure(size_t currentUsage, size_t threshold);
    void memoryStatsUpdated(const MemoryStats& stats);

private:
    FluentMemoryManager() = default;
    void checkMemoryPressure();

private:
    mutable QMutex m_statsMutex;
    MemoryStats m_stats;
    bool m_trackingEnabled{false};
    size_t m_pressureThreshold{512 * 1024 * 1024};  // 512MB
};

// RAII memory tracker
class FluentMemoryTracker {
public:
    explicit FluentMemoryTracker(const QString& category)
        : m_category(category), m_initialUsage(getCurrentMemoryUsage()) {}

    ~FluentMemoryTracker() {
        size_t finalUsage = getCurrentMemoryUsage();
        if (finalUsage > m_initialUsage) {
            FluentMemoryManager::instance().trackAllocation(
                finalUsage - m_initialUsage, m_category);
        } else if (m_initialUsage > finalUsage) {
            FluentMemoryManager::instance().trackDeallocation(
                m_initialUsage - finalUsage, m_category);
        }
    }

private:
    size_t getCurrentMemoryUsage() const;

private:
    QString m_category;
    size_t m_initialUsage;
};

// Smart pointer with memory pool allocation
template <typename T>
class FluentPoolPtr {
public:
    FluentPoolPtr() = default;

    template <typename... Args>
    explicit FluentPoolPtr(Args&&... args) {
        auto& pool = FluentMemoryManager::instance().getPool<T>();
        m_ptr = pool.construct(std::forward<Args>(args)...);
    }

    ~FluentPoolPtr() { reset(); }

    // Move semantics
    FluentPoolPtr(FluentPoolPtr&& other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }

    FluentPoolPtr& operator=(FluentPoolPtr&& other) noexcept {
        if (this != &other) {
            reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    // No copy semantics
    FluentPoolPtr(const FluentPoolPtr&) = delete;
    FluentPoolPtr& operator=(const FluentPoolPtr&) = delete;

    // Access
    T* get() const { return m_ptr; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }

    // Boolean conversion
    explicit operator bool() const { return m_ptr != nullptr; }

    // Reset
    void reset() {
        if (m_ptr) {
            auto& pool = FluentMemoryManager::instance().getPool<T>();
            pool.destroy(m_ptr);
            m_ptr = nullptr;
        }
    }

    // Release ownership
    T* release() {
        T* result = m_ptr;
        m_ptr = nullptr;
        return result;
    }

private:
    T* m_ptr{nullptr};
};

// Factory function for pool-allocated objects
template <typename T, typename... Args>
FluentPoolPtr<T> makeFluentPoolPtr(Args&&... args) {
    return FluentPoolPtr<T>(std::forward<Args>(args)...);
}

// Memory optimization macros
#define FLUENT_TRACK_MEMORY(category) \
    FluentQt::Core::FluentMemoryTracker _memTracker(category)

#define FLUENT_POOL_ALLOCATE(Type, ...) \
    FluentQt::Core::makeFluentPoolPtr<Type>(__VA_ARGS__)

}  // namespace FluentQt::Core
