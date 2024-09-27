#include "gtest/gtest.h"

#include "chunked_list.hpp"  // Include your chunked_list implementation header file

// Test Fixture Class
class ChunkedListTest : public ::testing::Test {
protected:
    container::chunked_list::ChunkedList<int>* plist;

    void SetUp() override {
        // Create a chunked_list with int items
        plist = new container::chunked_list::ChunkedList<int>(1024);
    }

    void TearDown() override {
        // Clean up the chunked_list after each test
        delete plist;
    }
};

// Test: Adding and retrieving items from the chunked_list
TEST_F(ChunkedListTest, AddAndRetrieveItem) {
    int item1 = 42;
    int item2 = 99;

    plist->add(item1);
	EXPECT_EQ(plist->size(), 1UL); // count == 1
	plist->emplace(item2);
	EXPECT_EQ(plist->size(), 2UL); // count == 2

    EXPECT_EQ(plist->at(0), 42);
    EXPECT_EQ((*plist)[1], 99);
	
	plist->at(0) = 11;
	(*plist)[1] = 15;
	EXPECT_EQ(plist->at(0), 11);
    EXPECT_EQ((*plist)[1], 15);
}

// Test: Removing an item from the chunked_list
TEST_F(ChunkedListTest, RemoveItem) {
    int item1 = 42;
    int item2 = 99;

    plist->add(item1);
    plist->add(item2);

    // Remove the first item (42)
    plist->remove(0);
	EXPECT_EQ(plist->size(), 1UL); // count == 1


    // After removal, the item plist->at index 0 should now be 99
    EXPECT_EQ(plist->at(0), 99);
}

// Test: Accessing an invalid index
TEST_F(ChunkedListTest, AccessInvalidIndex) {
    // No items added yet, so accessing any index should fail
    EXPECT_THROW(plist->at(0), std::out_of_range);

    int item1 = 42;
    plist->add(item1);

    // There is only one item, so accessing index 1 should fail
    EXPECT_THROW(plist->at(1), std::out_of_range);
}

// Test: Removing an invalid index
TEST_F(ChunkedListTest, RemoveInvalidIndex) {
    // Try removing from an empty list
    EXPECT_THROW(plist->at(0), std::out_of_range);

    int item1 = 42;
    plist->add(item1);

    // Remove should succeed plist->at index 0
    plist->remove(0);

    // Now the list is empty again, removing plist->at index 0 should fail
    EXPECT_THROW(plist->at(0), std::out_of_range);
}

// Test: Clearing the chunked_list
TEST_F(ChunkedListTest, ClearChunkedList) {
    int item1 = 42;
    int item2 = 99;

    plist->add(item1);
    plist->add(item2);

    plist->clear();  // Clear all items
	
	EXPECT_EQ(plist->size(), 0UL); // count == 0

    // After clearing, accessing any index should fail
    EXPECT_THROW(plist->at(0), std::out_of_range);

    // The list should still be operational, so we can plist->add new items
    plist->add(item1);
    EXPECT_EQ(plist->at(0), 42);
}

// Test: Adding and retrieving items from the chunked_list
TEST_F(ChunkedListTest, IteratorTests) {
	int COUNT = CHUNKED_LIST_CHUNK_SIZE / sizeof(int) * 4;

 	for(int idx=0; idx < COUNT; ++idx) {
		plist->add(idx);
	}
	
	int cur_value = 0;
	for(auto it=plist->begin(); it != plist->end(); ++it) {
		ASSERT_EQ(*it, cur_value++);
	}
    EXPECT_EQ((size_t)cur_value, plist->size());
	
    cur_value = 0;
	for(int& value : *plist) {
		ASSERT_EQ(value, cur_value++);
	}
    EXPECT_EQ((size_t)cur_value, plist->size());

    auto it1 = plist->begin();
    auto it2 = plist->begin();
    EXPECT_EQ(it1, it2);

    for (size_t i = 0; i < plist->size(); i++)
    {
        ASSERT_EQ(++it1, ++it2);
    }
}

TEST_F(ChunkedListTest, AddAndRetrieveRemoveManyItem) {
    int COUNT = CHUNKED_LIST_CHUNK_SIZE / sizeof(int) * 4;

    for (int idx = 0; idx < COUNT; ++idx) {
        plist->add(idx);
        ASSERT_EQ(plist->at(idx), idx);
        EXPECT_EQ(plist->size(), (size_t)(idx + 1));
    }

    // Remove 
    int idx = CHUNKED_LIST_CHUNK_SIZE / sizeof(int) * 2 + 4;
    plist->remove(idx);
    EXPECT_EQ(plist->size(), (size_t)(COUNT - 1));

    // After removal, the item plist->at index should now be idx+1
    EXPECT_EQ(plist->at(idx), idx + 1);

    // Remove 
    idx = CHUNKED_LIST_CHUNK_SIZE / sizeof(int) * 1 + 100;
    plist->remove(idx);
    EXPECT_EQ(plist->size(), (size_t)(COUNT - 2));

    // After removal, the item plist->at index (COUNT - 2) should now be COUNT
    EXPECT_EQ(plist->at(COUNT - 2 - 1), COUNT - 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
