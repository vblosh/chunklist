#ifndef CHUNKED_LIST_HPP
#define CHUNKED_LIST_HPP

#include <stdexcept>
#include <utility>
#include "chunked_list.h"  
#include "chunked_list_iterator.h"

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
            destroy_all_items();  // Invoke ~T() on every live element before freeing chunks
            chunked_list_destroy(chunked_list_); // Clean up the existing chunked_list if it owns it
        }
    }

    // Attach to an existing C-style chunked_list
    void attach(CHUNKED_LIST_HANDLE list, bool own_container=false) {
        if (own_container_ && chunked_list_) {
            destroy_all_items();
            chunked_list_destroy(chunked_list_); // Clean up the existing chunked_list if it owns it
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
        // Do NOT use chunked_list_add here: it memcpy's raw bytes, which would
        // bitwise-duplicate any resource T owns (e.g. heap buffers) instead of
        // invoking T's copy constructor. That leads to double-free once ~T()
        // is invoked on removal/clear/destruction. Allocate the slot and
        // copy-construct into it instead.
        void* newItemPtr = nullptr;
        if (chunked_list_expand(chunked_list_, &newItemPtr) != CHUNKED_LIST_SUCCESS) {
            throw std::bad_alloc();
        }
        new (newItemPtr) T(item);
    }

    // Get an item at a specific index as a reference
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

    // Remove an item at a specific index
    void remove(size_t index) {
        const int result = chunked_list_remove_managed(
            chunked_list_,
            index,
            [](void* destination, void* source) {
                *reinterpret_cast<T*>(destination) = std::move(*reinterpret_cast<T*>(source));
            },
            [](void* item) {
                reinterpret_cast<T*>(item)->~T();
            });
        if (result != CHUNKED_LIST_SUCCESS) {
            throw std::out_of_range("Failed to remove item: Index out of range.");
        }
    }

    // Clear the chunked_list
    void clear() {
        destroy_all_items();
        if (chunked_list_clear(chunked_list_) != CHUNKED_LIST_SUCCESS) {
            throw std::runtime_error("Failed to clear chunked_list.");
        }
    }

    // Get the size of the chunked_list (number of items)
    size_t size() const {
        return chunked_list_count(chunked_list_);  // Use the chunked_list_count function to get the size
    }
	
class iterator {
	public:
	    // Iterator traits
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

    // Default constructor: produces a singular end iterator (required for
    // ForwardIterator per [forward.iterators]).
    iterator() : c_iterator(nullptr), currentItem(nullptr) {}

    iterator(const iterator& other)
        : c_iterator(chunked_list_iterator_clone(other.c_iterator)), currentItem(nullptr) {
        if (other.c_iterator && !c_iterator) {
            throw std::bad_alloc();
        }
    }

    iterator& operator=(const iterator& other) {
        if (this == &other) {
            return *this;
        }

        CHUNKED_LIST_ITERATOR_HANDLE clone = chunked_list_iterator_clone(other.c_iterator);
        if (other.c_iterator && !clone) {
            throw std::bad_alloc();
        }
        chunked_list_iterator_destroy(c_iterator);
        c_iterator = clone;
        currentItem = nullptr;
        return *this;
    }

    iterator(iterator&& other) noexcept
        : c_iterator(other.c_iterator), currentItem(other.currentItem) {
        other.c_iterator = nullptr;
        other.currentItem = nullptr;
    }

    iterator& operator=(iterator&& other) noexcept {
        if (this != &other) {
            chunked_list_iterator_destroy(c_iterator);
            c_iterator = other.c_iterator;
            currentItem = other.currentItem;
            other.c_iterator = nullptr;
            other.currentItem = nullptr;
        }
        return *this;
    }

    // Constructor: Takes a handle to a chunked list and initializes the iterator
    // or create an end iterator if the handle is null
    iterator(ChunkedList<T>* list)
        : currentItem(nullptr) 
    {
        if (list) {
            c_iterator = chunked_list_iterator_create(list->chunked_list_);
            if (c_iterator == nullptr)
                throw std::bad_alloc();
        }
        else { // create end iterator
            c_iterator = nullptr;
        }
    }

    // Destructor: Cleans up the C iterator
    ~iterator() {
		chunked_list_iterator_destroy(c_iterator);
    }

    // Dereference operator to access the current item
    T& operator*() {
		if(CHUNKED_LIST_ITERATOR_SUCCESS != chunked_list_iterator_get(c_iterator, (void**)&currentItem))
			throw std::out_of_range("Failed to get item: Index out of range.");

        return *currentItem;
    }

    // Pointer access operator to access the current item
    T* operator->() {
 		if(chunked_list_iterator_get(c_iterator, (void**)&currentItem) != CHUNKED_LIST_ITERATOR_SUCCESS)
			throw std::out_of_range("Failed to get item: Index out of range.");

       return currentItem;
    }

    // Prefix increment to move to the next item
    iterator& operator++() {
        if(chunked_list_iterator_next(c_iterator) != CHUNKED_LIST_ITERATOR_SUCCESS)
			throw std::out_of_range("Failed to move iterator: Index out of range.");
        return *this;
    }

    // Post-increment
    iterator operator++(int) {
        iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    size_t index() const {
        return chunked_list_iterator_get_index(c_iterator);
    }

	// Equality comparison
	bool operator==(const iterator& other) const {
        // Two singular (default-constructed) iterators are equal.
        if (!c_iterator && !other.c_iterator) return true;
        if (other.c_iterator)
            return index() == other.index();
        else // other is an end iterator
            return chunked_list_iterator_is_end(c_iterator) == 1;
	}

	// Inequality comparison
	bool operator!=(const iterator& other) const {
		return !operator==(other);
	}
	
private:
    CHUNKED_LIST_ITERATOR_HANDLE c_iterator; // The C iterator handle
	T* currentItem;    
};

    // Return iterator pointing to the first element
    iterator begin() {
        return iterator(this);
    }

    // Return iterator pointing to one past the last element
    iterator end() {
        return iterator(nullptr);
    }
	
private:
    // Invoke ~T() on every live element. Walks the list via the C iterator
    // (linear, not chunked_list_at-in-a-loop which would be O(n^2) since each
    // call re-walks from head) to cover items spread across multiple chunks.
    // Safe to call on an empty list.
    void destroy_all_items() {
        if (!chunked_list_) {
            return;
        }
        CHUNKED_LIST_ITERATOR_HANDLE it = chunked_list_iterator_create(chunked_list_);
        if (!it) {
            return;
        }
        void* item_ptr = nullptr;
        while (chunked_list_iterator_get(it, &item_ptr) == CHUNKED_LIST_ITERATOR_SUCCESS) {
            reinterpret_cast<T*>(item_ptr)->~T();
            if (chunked_list_iterator_next(it) != CHUNKED_LIST_ITERATOR_SUCCESS) {
                break;
            }
        }
        chunked_list_iterator_destroy(it);
    }

    CHUNKED_LIST_HANDLE chunked_list_;       // The handle to the C-style chunked_list
    bool own_container_;     // Flag to indicate ownership of the chunked_list
};

	}
}

#endif // CHUNKED_LIST_HPP
