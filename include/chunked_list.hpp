#ifndef CHUNKED_LIST_HPP
#define CHUNKED_LIST_HPP

#include <stdexcept>
#include "chunked_list.h"  // Include the C container implementation

namespace container {
	namespace chunked_list {

// C++ Template class wrapping the C chunked_list
template <typename T>
class ChunkedList {
public:
    // Constructor
    ChunkedList(size_t chunk_size = CHUNKED_LIST_CHUNK_SIZE) : chunked_list_(nullptr), own_container_(true) {
        chunked_list_ = chunked_list_create(sizeof(T), chunk_size);
        if (!chunked_list_) {
            throw std::runtime_error("Failed to create chunked_list.");
        }
    }

    // Destructor
    ~ChunkedList() {
        if (own_container_ && chunked_list_) {
            chunked_list_delete(chunked_list_); // Clean up the existing chunked_list if it owns it
        }
    }

    // Attach to an existing C-style chunked_list
    void attach(CHUNKED_LIST_HANDLE list, bool own_container=false) {
        if (own_container_ && chunked_list_) {
            chunked_list_delete(chunked_list_); // Clean up the existing chunked_list if it owns it
        }
        chunked_list_ = list;
        own_container_ = own_container;  
    }

	// Emplace a new object in the chunk list using perfect forwarding
    template <typename... Args>
    void emplace(Args&&... args) {
        void* newItemPtr = nullptr;

        // Expand the chunk list to allocate space for the new item
        if (chunked_list_expand(chunked_list_, &newItemPtr) != CHUNKED_LIST_SUCCESS) {
            throw std::bad_alloc();
        }

        // Construct the new object in the allocated space using placement new
        new (newItemPtr) T(std::forward<Args>(args)...);
    }
	
    // Add an item to the chunked_list
    void add(const T& item) {
        if (chunked_list_add(chunked_list_, (void*)&item) != CHUNKED_LIST_SUCCESS) {
            throw std::bad_alloc();
        }
    }

    // Get an item chunked_list_at a specific index as a reference
    T& at(size_t index) {
        void* item_ptr = nullptr;
        if (chunked_list_at(chunked_list_, index, &item_ptr) != CHUNKED_LIST_SUCCESS) {
            throw std::out_of_range("Index out of range.");
        }
        return *reinterpret_cast<T*>(item_ptr);  // Return the item as reference
    }

    // Operator[] to access items by index
    T& operator[](size_t index) {
        return at(index);  // Use the chunked_list_at method to retrieve the item
    }

    // Remove an item chunked_list_at a specific index
    void remove(size_t index) {
        if (chunked_list_remove(chunked_list_, index) != CHUNKED_LIST_SUCCESS) {
            throw std::out_of_range("Failed to remove item: Index out of range.");
        }
    }

    // Clear the chunked_list
    void clear() {
        if (chunked_list_clear(chunked_list_) != CHUNKED_LIST_SUCCESS) {
            throw std::runtime_error("Failed to clear chunked_list.");
        }
    }

    // Get the size of the chunked_list (number of items)
    size_t size() const {
        return chunked_list_count(chunked_list_);  // Use the chunked_list_count function to get the size
    }

private:
    CHUNKED_LIST_HANDLE chunked_list_;       // The handle to the C-style chunked_list
    bool own_container_;     // Flag to indicate ownership of the chunked_list
};

	}
}

#endif // CHUNKED_LIST_HPP