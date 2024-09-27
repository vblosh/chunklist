#include <stdlib.h>
#include <string.h>

#include "chunked_list.h"
#include "chunked_list_imp.h"

// Function to create a new chunked_list
CHUNKED_LIST_HANDLE chunked_list_create(size_t item_size, size_t chunk_size) {
    ChunkedList* chunked_list = (ChunkedList*)malloc(sizeof(ChunkedList));
    if (!chunked_list) {
        return NULL;
    }

    chunked_list->item_size = item_size;
    chunked_list->chunk_size = chunk_size;
    chunked_list->total_items = 0;
    chunked_list->head = NULL;
    chunked_list->tail = NULL;

    return chunked_list;
}

// Function to delete the chunked_list and free all resources
int chunked_list_destroy(CHUNKED_LIST_HANDLE list) {
    chunked_list_clear(list);
    free(list);
    return CHUNKED_LIST_SUCCESS;
}

int chunked_list_clear(CHUNKED_LIST_HANDLE list){
    ChunkedList* chunked_list = (ChunkedList*)list;
    Chunk* current = chunked_list->head;
    while (current) {
        Chunk* next = current->next;
        free(current);
        current = next;
    }
	chunked_list->total_items = 0;
    chunked_list->head = NULL;
    chunked_list->tail = NULL;
	
    return CHUNKED_LIST_SUCCESS;
}

// Function to create a new chunk
Chunk* create_chunk(size_t chunk_size) {
    Chunk* chunk = (Chunk*)malloc(sizeof(Chunk) + chunk_size);
    if (!chunk) {
        return NULL;
    }
    chunk->next = NULL;
    chunk->used = 0;
    return chunk;
}

// Function to expands the chunked list for a new item
int chunked_list_expand(CHUNKED_LIST_HANDLE list, void** pnewItem) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    
    // Check if the tail chunk is full or doesn't exist
    if (!chunked_list->tail || chunked_list->tail->used + chunked_list->item_size > chunked_list->chunk_size) {
        Chunk* new_chunk = create_chunk(chunked_list->chunk_size);
        if (!new_chunk) {
            return CHUNKED_LIST_ERROR_ALLOCATION_FAILED;
        }
        
        if (!chunked_list->head) {
            chunked_list->head = new_chunk;
        } else {
            chunked_list->tail->next = new_chunk;
        }
        
        chunked_list->tail = new_chunk;
    }
    
    // Expand the current tail chunk
    void* destination = chunked_list->tail->data + chunked_list->tail->used;
    chunked_list->tail->used += chunked_list->item_size;
    chunked_list->total_items++;
	*pnewItem = destination;
	
    return CHUNKED_LIST_SUCCESS;
}

// Function to add an item to the chunked_list
int chunked_list_add(CHUNKED_LIST_HANDLE list, void* item) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    
    // Add the item to the current tail chunk
    void* destination; 
	int error_code = chunked_list_expand(list, &destination);
	if(CHUNKED_LIST_SUCCESS != error_code)
		return error_code;
	
    memcpy(destination, item, chunked_list->item_size);
    
    return CHUNKED_LIST_SUCCESS;
}

// Function to retrieve an item at a specific index
int chunked_list_at(CHUNKED_LIST_HANDLE list, size_t index, void** item) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    if (index >= chunked_list->total_items) {
        return CHUNKED_LIST_ERROR_INVALID_INDEX;
    }

    size_t items_to_skip = index;
    Chunk* current_chunk = chunked_list->head;
    
    while (current_chunk) {
        size_t chunk_items = current_chunk->used / chunked_list->item_size;
        if (items_to_skip < chunk_items) {
            *item = (void*)(current_chunk->data + items_to_skip * chunked_list->item_size);
            return CHUNKED_LIST_SUCCESS;
        }
        items_to_skip -= chunk_items;
        current_chunk = current_chunk->next;
    }

    return CHUNKED_LIST_ERROR_INVALID_INDEX;
}

// Function to remove an item at a specific index
int chunked_list_remove(CHUNKED_LIST_HANDLE list, size_t index) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    if (index >= chunked_list->total_items) {
        return CHUNKED_LIST_ERROR_INVALID_INDEX;
    }

    size_t items_to_skip = index;
    Chunk* current_chunk = chunked_list->head;
    
    while (current_chunk) {
        size_t chunk_items = current_chunk->used / chunked_list->item_size;
        if (items_to_skip < chunk_items) {
            // Found the chunk containing the item to chunked_list_remove
            char* item_to_remove = current_chunk->data + items_to_skip * chunked_list->item_size;
            char* next_item = item_to_remove + chunked_list->item_size;
            
            // Shift all items in the chunk after the item_to_remove to fill the gap
            size_t remaining_items_in_chunk = chunk_items - items_to_skip - 1;  // Items after the removed one
            if (remaining_items_in_chunk > 0) {
                memmove(item_to_remove, next_item, remaining_items_in_chunk * chunked_list->item_size);
            }
            
            // Reduce the used size in the current chunk
            current_chunk->used -= chunked_list->item_size;
            chunked_list->total_items--;
            
            return CHUNKED_LIST_SUCCESS;
        }
        items_to_skip -= chunk_items;
        current_chunk = current_chunk->next;
    }

    return CHUNKED_LIST_ERROR_INVALID_INDEX;
}

// Function to get the total number of items in the chunked_list
size_t chunked_list_count(CHUNKED_LIST_HANDLE list) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    return chunked_list->total_items;
}

