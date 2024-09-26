#ifndef CHUNKED_LIST_IMP_H
#define CHUNKED_LIST_IMP_H

typedef struct Chunk {
    struct Chunk* next;
    size_t used;  // Number of bytes used in this chunk
    char data[];  // Flexible array member to hold items
} Chunk;

typedef struct {
    size_t item_size;    // Size of each item
	size_t chunk_size;	 // Size of each chunk
    size_t total_items;  // Total number of items in the chunked_list
    Chunk* head;         // Pointer to the first chunk
    Chunk* tail;         // Pointer to the last chunk
} ChunkedList;

#endif // CHUNKED_LIST_IMP_H
