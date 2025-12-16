#define MESHLOADER_IMPLEMENTATION
#include "meshloader/meshloader.h"

#include <iostream>

void sanity_test();
void test_triangle();

int main() {
	std::cout << "Running meshloader tests..." << std::endl;

	sanity_test();

	std::cout << "All tests passed." << std::endl;
	return 0;
}