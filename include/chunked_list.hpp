#ifndef CHUNKED_LIST_HPP
#define CHUNKED_LIST_HPP

#include <stdexcept>
#include "chunked_list.h"  // Include the C container implementation

namespace container {
	namespace chunklist {

// C++ Template class wrapping the C chunklist
template <typename T>
class Chunklist {
public:
    // Constructor
    Chunklist() : chunklist_(nullptr), own_container_(true) {
        chunklist_ = chunklist_create(sizeof(T));
        if (!chunklist_) {
            throw std::runtime_error("Failed to create chunklist.");
        }
    }

    // Destructor
    ~Chunklist() {
        if (own_container_ && chunklist_) {
            chunklist_delete(chunklist_); // Clean up the existing chunklist if it owns it
        }
    }

    // Attach to an existing C-style chunklist
    void attach(CHUNKLIST_HANDLE list, bool own_container=false) {
        if (own_container_ && chunklist_) {
            chunklist_delete(chunklist_); // Clean up the existing chunklist if it owns it
        }
        chunklist_ = list;
        own_container_ = own_container;  
    }

    // Add an item to the chunklist
    void add(const T& item) {
        if (chunklist_add(chunklist_, (void*)&item) != CHUNKLIST_SUCCESS) {
            throw std::runtime_error("Failed to add item to chunklist.");
        }
    }

    // Get an item chunklist_at a specific index as a reference
    T& at(size_t index) {
        void* item_ptr = nullptr;
        if (chunklist_at(chunklist_, index, &item_ptr) != CHUNKLIST_SUCCESS) {
            throw std::out_of_range("Index out of range.");
        }
        return *(T*)item_ptr;  // Return the item as reference
    }

    // Operator[] to access items by index
    T& operator[](size_t index) {
        return at(index);  // Use the chunklist_at method to retrieve the item
    }

    // Remove an item chunklist_at a specific index
    void remove(size_t index) {
        if (chunklist_remove(chunklist_, index) != CHUNKLIST_SUCCESS) {
            throw std::out_of_range("Failed to remove item: Index out of range.");
        }
    }

    // Clear the chunklist
    void clear() {
        if (chunklist_clear(chunklist_) != CHUNKLIST_SUCCESS) {
            throw std::runtime_error("Failed to clear chunklist.");
        }
    }

    // Get the size of the chunklist (number of items)
    size_t size() const {
        return chunklist_count(chunklist_);  // Use the chunklist_count function to get the size
    }

private:
    CHUNKLIST_HANDLE chunklist_;       // The handle to the C-style chunklist
    bool own_container_;     // Flag to indicate ownership of the chunklist
};

	}
}

#endif // CHUNKED_LIST_HPP