#include <stdlib.h>
#include "chunked_list_iterator.h"
#include "chunked_list_imp.h"

// Internal iterator structure, hidden from the user
typedef struct {
    ChunkedList* list;         // The chunked list being iterated over
    Chunk* current_chunk;      // The current chunk being iterated
    size_t chunk_pos;          // The position inside the current chunk
    size_t global_index;       // The global position in the entire list
} ChunkListIterator;

CHUNKED_LIST_ITERATOR_HANDLE chunked_list_iterator_create(CHUNKED_LIST_HANDLE list) {
    if (!list) {
        return NULL;
    }
    ChunkListIterator* iterator = (ChunkListIterator*)malloc(sizeof(ChunkListIterator));
    if (!iterator) {
        return NULL;
    }

    ChunkedList* chunked_list = (ChunkedList*)list;
    iterator->list = chunked_list;
    iterator->current_chunk = chunked_list->head;
    iterator->chunk_pos = 0;
    iterator->global_index = 0;

    return iterator;
}

CHUNKED_LIST_ITERATOR_HANDLE chunked_list_iterator_clone(CHUNKED_LIST_ITERATOR_HANDLE iterator_handle) {
    ChunkListIterator* iterator = (ChunkListIterator*)iterator_handle;
    if (!iterator) {
        return NULL;
    }

    ChunkListIterator* clone = (ChunkListIterator*)malloc(sizeof(ChunkListIterator));
    if (!clone) {
        return NULL;
    }

    *clone = *iterator;
    return clone;
}

void chunked_list_iterator_destroy(CHUNKED_LIST_ITERATOR_HANDLE iterator_handle) {
    ChunkListIterator* iterator = (ChunkListIterator*)iterator_handle;
    if (iterator) {
        free(iterator);
    }
}

int chunked_list_iterator_get(CHUNKED_LIST_ITERATOR_HANDLE iterator_handle, void** item) {
    ChunkListIterator* iterator = (ChunkListIterator*)iterator_handle;
    if (!iterator || !item) {
        return CHUNKED_LIST_ITERATOR_ERROR_INVALID_INDEX;
    }

    if (!iterator->current_chunk || iterator->global_index >= iterator->list->total_items) {
        return CHUNKED_LIST_ITERATOR_ERROR_INVALID_INDEX;  // Out of bounds
    }

    *item = (void*)(iterator->current_chunk->data + iterator->chunk_pos * iterator->list->item_size);
    return CHUNKED_LIST_ITERATOR_SUCCESS;
}

int chunked_list_iterator_next(CHUNKED_LIST_ITERATOR_HANDLE iterator_handle) {
    ChunkListIterator* iterator = (ChunkListIterator*)iterator_handle;
    if (!iterator) {
        return CHUNKED_LIST_ITERATOR_ERROR_INVALID_INDEX;
    }

    if (!iterator->current_chunk || iterator->global_index >= iterator->list->total_items) {
        return CHUNKED_LIST_ITERATOR_ERROR_INVALID_INDEX;  // No more items
    }

    iterator->chunk_pos++;
    size_t items_in_current_chunk = iterator->current_chunk->used / iterator->list->item_size;

    // Move to the next chunk if necessary
    if (iterator->chunk_pos >= items_in_current_chunk) {
        iterator->current_chunk = iterator->current_chunk->next;
        iterator->chunk_pos = 0;
    }

    iterator->global_index++;
    return CHUNKED_LIST_ITERATOR_SUCCESS;
}

int chunked_list_iterator_is_end(CHUNKED_LIST_ITERATOR_HANDLE iterator_handle) {
    ChunkListIterator* iterator = (ChunkListIterator*)iterator_handle;
    if (!iterator) {
        return 1;
    }
    return (iterator->global_index >= iterator->list->total_items) ? 1 : 0;
}

size_t chunked_list_iterator_get_index(CHUNKED_LIST_ITERATOR_HANDLE iterator_handle)
{
    ChunkListIterator* iterator = (ChunkListIterator*)iterator_handle;
    if (!iterator) {
        return (size_t)-1;
    }
    return iterator->global_index;
}
