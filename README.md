# Chunked List

A **chunked list** is a data structure that stores elements in chunks, making it possible to handle dynamic lists of items without reallocating memory for each element individually. This implementation is written in C and uses a fixed-size chunk for storage, with an efficient mechanism for adding, removing, and accessing items.

## Features

- Dynamically grows in chunks, avoiding frequent memory allocations.
- Efficient addition and deletion of elements.
- Supports iteration through an opaque handle-based iterator.
- Provides a C++ wrapper for seamless integration into C++ projects.
- Includes unit tests written using Google Test.

## Installation

### Requirements

- **C Compiler**: GCC or any other modern C compiler.
- **Make**: For building the project.
- **Google Test**: (Optional) For running unit tests.

### Building the Library and Tests

1. Clone the repository:

```bash
git clone https://github.com/yourusername/chunked-list.git
cd chunked-list
```
2. Build the static library and the test executable:
```bash
make
```
3. To run the tests:
```bash
make tests
```
4. To clean the build files:
```bash
make clean
```
## Usage
### C Interface
1. Creating a Chunked List

```C
#include "chunked_list.h"

CHUNKED_LIST_HANDLE list = chunked_list_create(sizeof(MyStruct), chunk_size);
```
2. Adding Items
```C
MyStruct item;
chunked_list_add(list, &item);
```
3. Accessing Items

```C
MyStruct* item_ptr;
if (chunked_list_at(list, index, (void**)&item_ptr) == CHUNKED_LIST_SUCCESS) {
    // Access item_ptr
}
```
4. Removing Items

```C
chunked_list_remove(list, index);
```
5. Clearing the List

```C
chunked_list_clear(list);
```
6. Destroying the List

```C
chunked_list_destroy(list);
```
### C++ Interface
The C++ wrapper provides a more convenient interface for C++ users.

1. Using the C++ Wrapper
```cpp
Copy code
#include "chunked_list.hpp"

ChunkedList<MyStruct> list(sizeof(MyStruct)*100); // chunk size
list.emplace(MyStruct{...});
MyStruct& item = list[0];
```
2. Iterator Example
```cpp
for (auto it = list.begin(); it != list.end(); ++it) {
    MyStruct& item = *it;
    // Process item
}
```
## API Reference
### C API
Function | Description
--------------------------------------------------------------------------|------------------------------------------------
CHUNKED_LIST_HANDLE chunked_list_create(size_t item_size, size_t chunk_size);|	Creates a chunked list with given chunk size.
int chunked_list_destroy(CHUNKED_LIST_HANDLE list);|	Deletes a chunked list and frees all resources.
int chunked_list_add(CHUNKED_LIST_HANDLE list, void* item);|	Adds a new item to the chunked list.
int chunked_list_expand(CHUNKED_LIST_HANDLE list, void** pnewItem);| Expands the chunked list for a new item and return the address of the item back.
int chunked_list_at(CHUNKED_LIST_HANDLE list, size_t index, void** item);|	Retrieves an item by index.
int chunked_list_remove(CHUNKED_LIST_HANDLE list, size_t index);|	Removes an item by index.
void chunked_list_clear(CHUNKED_LIST_HANDLE list);|	Clears all items from the list.
size_t chunked_list_count(CHUNKED_LIST_HANDLE list);| Gets the number of items in the chunked list.
### C++ API
The C++ wrapper provides a **ChunkedListWrapper<T>** class with methods:
Function | Description
--------------------------------------------------------------------------|------------------------------------------------
ChunkedList(size_t chunk_size = CHUNKED_LIST_CHUNK_SIZE);| Creates a chunked list with given chunk size.
void attach(CHUNKED_LIST_HANDLE list, bool own_container=false);| Attach to an existing C-style chunked_list.
add(T item);| Adds an item to the list.
template <typename... Args> void emplace(Args&&... args);| Emplace a new object in the chunk list using perfect forwarding
T& at(size_t index);| Accesses an item.
T& operator[](size_t index);| Overloaded for array-like access.
void remove(size_t index);| Removes an item by index.
void clear();| Clears all items from the list.
size_t size() const;| Gets the number of items in the chunked list.
begin(), end();| Iterator support.
### Testing
This project includes unit tests based on Google Test. After building, you can run the test executable:
```bash
./tests/test_chunked_list_c
./tests/test_chunked_list_cpp
```
Make sure you have Google Test installed for the tests to run.

### Contributing
Feel free to open issues and submit pull requests! Contributions are welcome.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
