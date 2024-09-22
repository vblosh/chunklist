#ifndef CHUNKED_LIST_H
#define CHUNKED_LIST_H

#include <stddef.h>  // For size_t

#ifdef __cplusplus
extern "C" {
#endif

/// Error code for successful operation
#define CHUNKED_LIST_SUCCESS 0

/// Error code for invalid index
#define CHUNKED_LIST_ERROR_INVALID_INDEX -1

/// Error code for memory allocation failure
#define CHUNKED_LIST_ERROR_ALLOCATION_FAILED -2

/// Chunk size definition (16 KB)
#define CHUNKED_LIST_CHUNK_SIZE (16 * 1024)

/// Opaque type for the chunked list handle
typedef void* CHUNKED_LIST_HANDLE;

/**
 * @brief Creates a new chunked list container.
 *
 * Allocates a new chunked list capable of holding items of a specified size.
 *
 * @param item_size The size of each item in the list.
 * @param chunk_size The size of each chunk in the list.
 * @return A handle to the new chunked list, or NULL if memory allocation fails.
 */
CHUNKED_LIST_HANDLE chunked_list_create(size_t item_size, size_t chunk_size);

/**
 * @brief Deletes a chunked list and frees all resources.
 *
 * Frees all memory and resources associated with the chunked list.
 *
 * @param list A handle to the chunked list.
 * @return CHUNKED_LIST_SUCCESS on success, or an error code on failure.
 */
int chunked_list_delete(CHUNKED_LIST_HANDLE list);

/**
 * @brief Retrieves an item at a specific index in the chunked list.
 *
 * Retrieves the item located chunked_list_at the given index.
 *
 * @param list A handle to the chunked list.
 * @param index The index of the item to retrieve.
 * @param item Pointer to a pointer where the retrieved item will be stored.
 * @return CHUNKED_LIST_SUCCESS on success, or CHUNKED_LIST_ERROR_INVALID_INDEX if the index is out of range.
 */
int chunked_list_at(CHUNKED_LIST_HANDLE list, size_t index, void** item);

/**
 * @brief Expands the chunked list for a new item.
 *
 * Expands the chunked list for a new item and return the address of the item back.
 *
 * @param list A handle to the chunked list.
 * @param item Pointer to a pointer where the retrieved item will be stored.
 * @return CHUNKED_LIST_SUCCESS on success, or CHUNKED_LIST_ERROR_ALLOCATION_FAILED if memory allocation fails.
 */
int chunked_list_expand(CHUNKED_LIST_HANDLE list, void** pnewItem);

/**
 * @brief Adds a new item to the chunked list.
 *
 * Adds the provided item to the end of the chunked list.
 *
 * @param list A handle to the chunked list.
 * @param item A pointer to the item to be added.
 * @return CHUNKED_LIST_SUCCESS on success, or CHUNKED_LIST_ERROR_ALLOCATION_FAILED if memory allocation fails.
 */
int chunked_list_add(CHUNKED_LIST_HANDLE list, void* item);

/**
 * @brief Removes an item from the chunked list chunked_list_at a specific index.
 *
 * Removes the item located chunked_list_at the specified index. The items in the same chunk are
 * shifted to fill the gap, but other chunks remain unaffected.
 *
 * @param list A handle to the chunked list.
 * @param index The index of the item to chunked_list_remove.
 * @return CHUNKED_LIST_SUCCESS on success, or CHUNKED_LIST_ERROR_INVALID_INDEX if the index is out of range.
 */
int chunked_list_remove(CHUNKED_LIST_HANDLE list, size_t index);

/**
 * @brief Clears all items from the chunked list.
 *
 * Removes all items from the chunked list, effectively resetting it.
 *
 * @param list A handle to the chunked list.
 * @return CHUNKED_LIST_SUCCESS on success, or an error code on failure.
 */
int chunked_list_clear(CHUNKED_LIST_HANDLE list);

/**
 * @brief Gets the number of items in the chunked list.
 *
 * Returns the total number of items currently stored in the chunked list.
 *
 * @param list A handle to the chunked list.
 * @return The number of items in the list.
 */
size_t chunked_list_count(CHUNKED_LIST_HANDLE list);

#ifdef __cplusplus
}
#endif

#endif // CHUNKED_LIST_H
