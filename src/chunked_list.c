#include <stdlib.h>
#include <string.h>

#include "chunked_list.h"

typedef struct Chunk {
    struct Chunk* next;
    size_t used;  // Number of bytes used in this chunk
    char data[];  // Flexible array member to hold items
} Chunk;

typedef struct {
    size_t item_size;    // Size of each item
    size_t total_items;  // Total number of items in the chunklist
    Chunk* head;         // Pointer to the first chunk
    Chunk* tail;         // Pointer to the last chunk
} ChunkedList;

// Function to create a new chunklist
CHUNKLIST_HANDLE chunklist_create(size_t item_size) {
    ChunkedList* chunked_list = (ChunkedList*)malloc(sizeof(ChunkedList));
    if (!chunked_list) {
        return NULL;
    }

    chunked_list->item_size = item_size;
    chunked_list->total_items = 0;
    chunked_list->head = NULL;
    chunked_list->tail = NULL;

    return chunked_list;
}

// Function to delete the chunklist and free all resources
int chunklist_delete(CHUNKLIST_HANDLE list) {
    chunklist_clear(list);
    free(list);
    return CHUNKLIST_SUCCESS;
}

int chunklist_clear(CHUNKLIST_HANDLE list){
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
	
    return CHUNKLIST_SUCCESS;
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
int chunklist_expand(CHUNKLIST_HANDLE list, void** pnewItem) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    
    // Check if the tail chunk is full or doesn't exist
    if (!chunked_list->tail || chunked_list->tail->used + chunked_list->item_size > CHUNKLIST_CHUNK_SIZE) {
        Chunk* new_chunk = create_chunk(CHUNKLIST_CHUNK_SIZE);
        if (!new_chunk) {
            return CHUNKLIST_ERROR_ALLOCATION_FAILED;
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
	
    return CHUNKLIST_SUCCESS;
}

// Function to add an item to the chunklist
int chunklist_add(CHUNKLIST_HANDLE list, void* item) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    
    // Add the item to the current tail chunk
    void* destination; 
	int error_code = chunklist_expand(list, &destination);
	if(CHUNKLIST_SUCCESS != error_code)
		return error_code;
	
    memcpy(destination, item, chunked_list->item_size);
    
    return CHUNKLIST_SUCCESS;
}

// Function to retrieve an item at a specific index
int chunklist_at(CHUNKLIST_HANDLE list, size_t index, void** item) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    if (index >= chunked_list->total_items) {
        return CHUNKLIST_ERROR_INVALID_INDEX;
    }

    size_t items_to_skip = index;
    Chunk* current_chunk = chunked_list->head;
    
    while (current_chunk) {
        size_t chunk_items = current_chunk->used / chunked_list->item_size;
        if (items_to_skip < chunk_items) {
            *item = (void*)(current_chunk->data + items_to_skip * chunked_list->item_size);
            return CHUNKLIST_SUCCESS;
        }
        items_to_skip -= chunk_items;
        current_chunk = current_chunk->next;
    }

    return CHUNKLIST_ERROR_INVALID_INDEX;
}

// Function to remove an item at a specific index
int chunklist_remove(CHUNKLIST_HANDLE list, size_t index) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    if (index >= chunked_list->total_items) {
        return CHUNKLIST_ERROR_INVALID_INDEX;
    }

    size_t items_to_skip = index;
    Chunk* current_chunk = chunked_list->head;
    
    while (current_chunk) {
        size_t chunk_items = current_chunk->used / chunked_list->item_size;
        if (items_to_skip < chunk_items) {
            // Found the chunk containing the item to chunklist_remove
            void* item_to_remove = current_chunk->data + items_to_skip * chunked_list->item_size;
            void* next_item = item_to_remove + chunked_list->item_size;
            
            // Shift all items in the chunk after the item_to_remove to fill the gap
            size_t remaining_items_in_chunk = chunk_items - items_to_skip - 1;  // Items after the removed one
            if (remaining_items_in_chunk > 0) {
                memmove(item_to_remove, next_item, remaining_items_in_chunk * chunked_list->item_size);
            }
            
            // Reduce the used size in the current chunk
            current_chunk->used -= chunked_list->item_size;
            chunked_list->total_items--;
            
            return CHUNKLIST_SUCCESS;
        }
        items_to_skip -= chunk_items;
        current_chunk = current_chunk->next;
    }

    return CHUNKLIST_ERROR_INVALID_INDEX;
}

// Function to get the total number of items in the chunklist
size_t chunklist_count(CHUNKLIST_HANDLE list) {
    ChunkedList* chunked_list = (ChunkedList*)list;
    return chunked_list->total_items;
}

