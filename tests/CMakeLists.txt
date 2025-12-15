add_executable(meshloader_tests
	"test_main.cpp"
	"test_obj_basic.cpp"
)

target_link_libraries(meshloader_tests PRIVATE meshloader)

target_compile_definitions(meshloader_tests PRIVATE 
	TEST_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

if (MSVC)
	target_compile_options(meshloader_tests PRIVATE /W4 /permissive-)
else()
	target_compile_options(meshloader_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()