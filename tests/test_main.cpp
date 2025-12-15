#define MESHLOADER_IMPLEMENTATION
#include "meshloader/meshloader.h"

#include <iostream>

void test_triangle();

int main() {
	std::cout << "Running meshloader tests..." << std::endl;

	test_triangle();

	std::cout << "All tests passed." << std::endl;
	return 0;
}