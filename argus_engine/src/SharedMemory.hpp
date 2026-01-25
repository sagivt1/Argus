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

// Defines the metadata structure at the start of the shared memory block.
// Layout must match the Python struct.pack/unpack format exactly.
struct BufferHeader {
    uint32_t frameId;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

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
    SharedMemoryWriter(const std::string& shm_name, size_t width, size_t height, size_t channels)
        : name(shm_name) {

         size_t pixelDataSize = width * height * channels;
         size = sizeof(BufferHeader) + pixelDataSize;

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
                // Ensure backing store is large enough for header + data
                shm->truncate(size);

            #endif
            // Map the newly created shared memory object into the process's virtual address space.
            region = mapped_region(*shm, read_write);

           

            // Write the initial metadata to the buffer's header.
            BufferHeader* header = static_cast<BufferHeader*>(region.get_address());
            header->frameId = 0;
            header->width = width;
            header->height = height;
            header->channels = channels;

            std::println("Synapse Active: {}x{} ({} ch) initialized.", width, height, channels);
        }
        catch (interprocess_exception& ex) {
            // Handle cases where the memory might already exist (e.g., race condition).
            std::println("Shared Memory Error: {}", ex.what());
             #ifdef _WIN32
                // As a fallback on Windows, try to open the existing segment.
                // Useful for hot-reloading or if the reader created the segment.
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

    /// @brief Copies a new frame of pixel data into the shared memory buffer.
    /// @details Increments the frameId to signal to readers that a new frame is available.
    void writeFrame(std::vector<uint8_t> const& pixelData) {
        uint8_t* base_addr = static_cast<uint8_t*>(region.get_address());
        BufferHeader* header = reinterpret_cast<BufferHeader*>(base_addr);
        uint8_t* data_ptr = base_addr + sizeof(BufferHeader);

        // Ensure the pixel data does not overflow the allocated buffer space.
        size_t  max_size = region.get_size() - sizeof(BufferHeader);
        if (pixelData.size() <= max_size) {
            // Direct memory copy avoids serialization overhead.
            std::memcpy(data_ptr, pixelData.data(), pixelData.size());

            // Increment the frameId to signal to the reader a new frame is available.
            // Note: No mutex used; assumes single writer/single reader with eventual consistency acceptable.
            header->frameId++;
        } 
    }
};