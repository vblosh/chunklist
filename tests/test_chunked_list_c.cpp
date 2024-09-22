#include "gtest/gtest.h"

#include "chunked_list.h"  // Include your chunked_list implementation header file

// Test Fixture Class
class ChunklistTest : public ::testing::Test {
protected:
    CHUNKLIST_HANDLE list;

    void SetUp() override {
        // Create a chunked_list with int items
        list = chunklist_create(sizeof(int), 1024);
    }

    void TearDown() override {
        // Clean up the chunked_list after each test
        chunklist_delete(list);
    }
};

// Test: Adding and retrieving items from the chunked_list
TEST_F(ChunklistTest, AddAndRetrieveItem) {
    int item1 = 42;
    int item2 = 99;
	int* pitem2;

    EXPECT_EQ(chunklist_add(list, &item1), CHUNKLIST_SUCCESS);
	EXPECT_EQ(chunklist_count(list), 1UL); // count == 1
    EXPECT_EQ(chunklist_expand(list, (void**)&pitem2), CHUNKLIST_SUCCESS);
	EXPECT_EQ(chunklist_count(list), 2UL); // count == 2
	*pitem2 = item2;

    int* retrieved_item;
    EXPECT_EQ(chunklist_at(list, 0, (void**)&retrieved_item), CHUNKLIST_SUCCESS);
    EXPECT_EQ(*retrieved_item, 42);

    EXPECT_EQ(chunklist_at(list, 1, (void**)&retrieved_item), CHUNKLIST_SUCCESS);
    EXPECT_EQ(*retrieved_item, 99);
}

// Test: Removing an item from the chunked_list
TEST_F(ChunklistTest, RemoveItem) {
    int item1 = 42;
    int item2 = 99;

    EXPECT_EQ(chunklist_add(list, &item1), CHUNKLIST_SUCCESS);
    EXPECT_EQ(chunklist_add(list, &item2), CHUNKLIST_SUCCESS);

    // Remove the first item (42)
    EXPECT_EQ(chunklist_remove(list, 0), CHUNKLIST_SUCCESS);
	EXPECT_EQ(chunklist_count(list), 1UL); // count == 1


    // After removal, the item chunklist_at index 0 should now be 99
    int* retrieved_item;
    EXPECT_EQ(chunklist_at(list, 0, (void**)&retrieved_item), CHUNKLIST_SUCCESS);
    EXPECT_EQ(*retrieved_item, 99);
}

// Test: Accessing an invalid index
TEST_F(ChunklistTest, AccessInvalidIndex) {
    int* retrieved_item;

    // No items added yet, so accessing any index should fail
    EXPECT_EQ(chunklist_at(list, 0, (void**)&retrieved_item), CHUNKLIST_ERROR_INVALID_INDEX);

    int item1 = 42;
    chunklist_add(list, &item1);

    // There is only one item, so accessing index 1 should fail
    EXPECT_EQ(chunklist_at(list, 1, (void**)&retrieved_item), CHUNKLIST_ERROR_INVALID_INDEX);
}

// Test: Removing an invalid index
TEST_F(ChunklistTest, RemoveInvalidIndex) {
    // Try removing from an empty list
    EXPECT_EQ(chunklist_remove(list, 0), CHUNKLIST_ERROR_INVALID_INDEX);

    int item1 = 42;
    chunklist_add(list, &item1);

    // Remove should succeed chunklist_at index 0
    EXPECT_EQ(chunklist_remove(list, 0), CHUNKLIST_SUCCESS);

    // Now the list is empty again, removing chunklist_at index 0 should fail
    EXPECT_EQ(chunklist_remove(list, 0), CHUNKLIST_ERROR_INVALID_INDEX);
}

// Test: Clearing the chunked_list
TEST_F(ChunklistTest, ClearChunklist) {
    int item1 = 42;
    int item2 = 99;

    EXPECT_EQ(chunklist_add(list, &item1), CHUNKLIST_SUCCESS);
    EXPECT_EQ(chunklist_add(list, &item2), CHUNKLIST_SUCCESS);

    chunklist_clear(list);  // Clear all items
	
	EXPECT_EQ(chunklist_count(list), 0UL); // count == 0

    // After clearing, accessing any index should fail
    int* retrieved_item;
    EXPECT_EQ(chunklist_at(list, 0, (void**)&retrieved_item), CHUNKLIST_ERROR_INVALID_INDEX);

    // The list should still be operational, so we can chunklist_add new items
    EXPECT_EQ(chunklist_add(list, &item1), CHUNKLIST_SUCCESS);
    EXPECT_EQ(chunklist_at(list, 0, (void**)&retrieved_item), CHUNKLIST_SUCCESS);
    EXPECT_EQ(*retrieved_item, 42);
}

// Test: Adding and retrieving items from the chunked_list
TEST_F(ChunklistTest, AddAndRetrieveRemoveManyItem) {
	int COUNT = CHUNKLIST_CHUNK_SIZE / sizeof(int) * 4;
 
    int* retrieved_item;

 	for(int idx=0; idx < COUNT; ++idx) {
		EXPECT_EQ(chunklist_add(list, &idx), CHUNKLIST_SUCCESS);
		EXPECT_EQ(chunklist_at(list, idx, (void**)&retrieved_item), CHUNKLIST_SUCCESS);
		EXPECT_EQ(*retrieved_item, idx);
		EXPECT_EQ(chunklist_count(list), (size_t)(idx+1));
	}
	
    // Remove 
	int idx = CHUNKLIST_CHUNK_SIZE / sizeof(int) * 2 + 4;
    EXPECT_EQ(chunklist_remove(list, idx), CHUNKLIST_SUCCESS);
	EXPECT_EQ(chunklist_count(list), (size_t)(COUNT - 1)); 

    // After removal, the item chunklist_at index should now be idx+1
    EXPECT_EQ(chunklist_at(list, idx, (void**)&retrieved_item), CHUNKLIST_SUCCESS);
    EXPECT_EQ(*retrieved_item, idx+1);
	
	    // Remove 
	idx = CHUNKLIST_CHUNK_SIZE / sizeof(int) * 1 + 100;
    EXPECT_EQ(chunklist_remove(list, idx), CHUNKLIST_SUCCESS);
	EXPECT_EQ(chunklist_count(list), (size_t)(COUNT - 2)); 

    // After removal, the item chunklist_at index (COUNT - 2) should now be COUNT
    EXPECT_EQ(chunklist_at(list, (COUNT - 2 - 1), (void**)&retrieved_item), CHUNKLIST_SUCCESS);
    EXPECT_EQ(*retrieved_item, COUNT - 1);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
