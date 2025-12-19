#define MESHLOADER_IMPLEMENTATION
#include "meshloader/meshloader.h"

#include <iostream>

#define EXPECT_TRUE(x) if (!(x)) return false;
#define EXPECT_EQ(a,b) if ((a)!=(b)) return false;

bool runAllTests();
bool sanity_test();
bool test_invalid_index();
bool test_short_face();
bool test_invalid_token();
bool test_empty_file();

int main() {
	std::cout << "Running meshloader tests..." << std::endl;

	if (!runAllTests()) {
		std::cerr << "Tests failed" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "All tests passed." << std::endl;
	return EXIT_SUCCESS;
}

bool runAllTests() {
	EXPECT_TRUE(sanity_test());

	// Error tests
	EXPECT_TRUE(test_invalid_index());
	EXPECT_TRUE(test_short_face());
	EXPECT_TRUE(test_invalid_token());
	EXPECT_TRUE(test_empty_file());

	return true;
}