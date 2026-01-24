#pragma once

#include <boost/interprocess/mapped_region.hpp>
#include <span>
#include <print>
#include <string>
#include <vector>

// --- Cross-Platform Shared Memory Abstraction ---
// Selects the appropriate Boost shared memory implementation based on the OS.
// windows_shared_memory is optimized for Windows, while shared_memory_object is POSIX-compliant.
#ifdef _WIN32
    #include <boost/interprocess/windows_shared_memory.hpp>
    using SharedMemObject = boost::interprocess::windows_shared_memory;
#else
    #include <boost/interprocess/shared_memory_object.hpp>
    using SharedMemObject = boost::interprocess::shared_memory_object;
#endif

using namespace boost::interprocess;

// Manages the lifecycle of a Boost shared memory segment for writing.
// This class abstracts away platform-specific details for creating and cleaning up
// the shared memory resource.
class SharedMemoryWriter {
    // A pointer to the platform-specific shared memory object.
    SharedMemObject* shm = nullptr;
    // A mapped region that makes the shared memory accessible in this process's address space.
    mapped_region region;
    std::string name;
    size_t size;

public:
    SharedMemoryWriter(const std::string& shm_name, size_t shm_size)
        : name(shm_name), size(shm_size) {

        // On POSIX systems, stale shared memory segments can persist after a crash.
        // This call ensures any old segment with the same name is removed before creation.
        #ifndef _WIN32
            shared_memory_object::remove(name.c_str());
        #endif

        try {
            // Create the shared memory object with platform-specific constructors.
            #ifdef _WIN32
                // Windows implementation allows specifying the size on creation.
                shm = new SharedMemObject(create_only, name.c_str(), read_write, size);
            #else
                // POSIX requires creating the object first, then setting its size.
                shm = new SharedMemObject(create_only, name.c_str(), read_write);
                shm->truncate(size);
            #endif

            // Map the newly created shared memory object into the process's virtual address space.
            region = mapped_region(*shm, read_write);

            std::println("Synapse Active: Shared Memory '{}' created. Size: {} bytes", name, size);
        }
        catch (interprocess_exception& ex) {
            // Handle cases where the memory might already exist (e.g., race condition).
            std::println("Error creating shared memory: {}. Attempting to open existing.", ex.what());
             #ifdef _WIN32
                // As a fallback on Windows, try to open the existing segment.
                shm = new SharedMemObject(open_only, name.c_str(), read_write);
                region = mapped_region(*shm, read_write);
            #endif
        }
    }

    // Ensures the shared memory resources are properly released.
    ~SharedMemoryWriter() {
        // Delete the heap-allocated shared memory object.
        if (shm) delete shm;
        // On POSIX, explicitly remove the named shared memory segment from the OS.
        #ifndef _WIN32
            shared_memory_object::remove(name.c_str());
        #endif
    }

    // Provides direct, non-owning access to the underlying memory buffer.
    std::span<unsigned char> getBuffer() {
        unsigned char* start = static_cast<unsigned char*>(region.get_address());
        return std::span<unsigned char>{start, size};
    }
};
