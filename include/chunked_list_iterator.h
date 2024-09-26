#ifndef CHUNKED_LIST_ITERATOR_H
#define CHUNKED_LIST_ITERATOR_H

#include "chunked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CHUNKED_LIST_ITERATOR_HANDLE;

/// Error code for successful operation
#define CHUNKED_LIST_ITERATOR_SUCCESS 0

/// Error code for invalid index
#define CHUNKED_LIST_ITERATOR_ERROR_INVALID_INDEX -1

/**
 * Create an iterator for the chunked list.
 * @param list The chunked list to iterate over.
 * @return A handle to the iterator, or NULL if creation fails.
 */
CHUNKED_LIST_ITERATOR_HANDLE chunked_list_iterator_create(CHUNKED_LIST_HANDLE list);

/**
 * Destroy the iterator.
 * @param iterator The iterator handle.
 */
void chunked_list_iterator_destroy(CHUNKED_LIST_ITERATOR_HANDLE iterator);

/**
 * Get the current item in the iterator.
 * @param iterator The iterator handle.
 * @param item A pointer to store the current item.
 * @return 0 if successful, -1 if the iterator is out of bounds.
 */
int chunked_list_iterator_get(CHUNKED_LIST_ITERATOR_HANDLE iterator, void** item);

/**
 * Move to the next item in the iterator.
 * @param iterator The iterator handle.
 * @return 0 if successful, -1 if the iterator has reached the end.
 */
int chunked_list_iterator_next(CHUNKED_LIST_ITERATOR_HANDLE iterator);

/**
 * Check if the iterator has reached the end of the list.
 * @param iterator The iterator handle.
 * @return 1 if the iterator is at the end, 0 otherwise.
 */
int chunked_list_iterator_is_end(CHUNKED_LIST_ITERATOR_HANDLE iterator);

/**
 * Return zero based position (index) in the list.
 * @param iterator The iterator handle.
 * @return zero based position (index) in the list.
 */
size_t chunked_list_iterator_get_index(CHUNKED_LIST_ITERATOR_HANDLE iterator);

#ifdef __cplusplus
}
#endif

#endif // CHUNKED_LIST_ITERATOR_H
