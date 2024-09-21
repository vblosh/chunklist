#ifndef CHUNKED_LIST_H
#define CHUNKED_LIST_H

#include <stddef.h>  // For size_t

#ifdef __cplusplus
extern "C" {
#endif

/// Error code for successful operation
#define CHUNKLIST_SUCCESS 0

/// Error code for invalid index
#define CHUNKLIST_ERROR_INVALID_INDEX -1

/// Error code for memory allocation failure
#define CHUNKLIST_ERROR_ALLOCATION_FAILED -2

/// Chunk size definition (128 KB)
#define CHUNKLIST_CHUNK_SIZE (128 * 1024)

/// Opaque type for the chunked list handle
typedef void* CHUNKLIST_HANDLE;

/**
 * @brief Creates a new chunked list container.
 *
 * Allocates a new chunked list capable of holding items of a specified size.
 *
 * @param item_size The size of each item in the list.
 * @return A handle to the new chunked list, or NULL if memory allocation fails.
 */
CHUNKLIST_HANDLE chunklist_create(size_t item_size);

/**
 * @brief Deletes a chunked list and frees all resources.
 *
 * Frees all memory and resources associated with the chunked list.
 *
 * @param list A handle to the chunked list.
 * @return CHUNKLIST_SUCCESS on success, or an error code on failure.
 */
int chunklist_delete(CHUNKLIST_HANDLE list);

/**
 * @brief Retrieves an item chunklist_at a specific index in the chunked list.
 *
 * Retrieves the item located chunklist_at the given index.
 *
 * @param list A handle to the chunked list.
 * @param index The index of the item to retrieve.
 * @param item Pointer to a pointer where the retrieved item will be stored.
 * @return CHUNKLIST_SUCCESS on success, or CHUNKLIST_ERROR_INVALID_INDEX if the index is out of range.
 */
int chunklist_at(CHUNKLIST_HANDLE list, size_t index, void** item);

/**
 * @brief Adds a new item to the chunked list.
 *
 * Adds the provided item to the end of the chunked list.
 *
 * @param list A handle to the chunked list.
 * @param item A pointer to the item to be added.
 * @return CHUNKLIST_SUCCESS on success, or CHUNKLIST_ERROR_ALLOCATION_FAILED if memory allocation fails.
 */
int chunklist_add(CHUNKLIST_HANDLE list, void* item);

/**
 * @brief Removes an item from the chunked list chunklist_at a specific index.
 *
 * Removes the item located chunklist_at the specified index. The items in the same chunk are
 * shifted to fill the gap, but other chunks remain unaffected.
 *
 * @param list A handle to the chunked list.
 * @param index The index of the item to chunklist_remove.
 * @return CHUNKLIST_SUCCESS on success, or CHUNKLIST_ERROR_INVALID_INDEX if the index is out of range.
 */
int chunklist_remove(CHUNKLIST_HANDLE list, size_t index);

/**
 * @brief Clears all items from the chunked list.
 *
 * Removes all items from the chunked list, effectively resetting it.
 *
 * @param list A handle to the chunked list.
 * @return CHUNKLIST_SUCCESS on success, or an error code on failure.
 */
int chunklist_clear(CHUNKLIST_HANDLE list);

/**
 * @brief Gets the number of items in the chunked list.
 *
 * Returns the total number of items currently stored in the chunked list.
 *
 * @param list A handle to the chunked list.
 * @return The number of items in the list.
 */
size_t chunklist_count(CHUNKLIST_HANDLE list);

#ifdef __cplusplus
}
#endif

#endif // CHUNKED_LIST_H
