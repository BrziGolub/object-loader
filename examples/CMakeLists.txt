add_executable (examples "load_and_print.cpp")

target_link_libraries(examples PRIVATE meshloader)

target_compile_definitions(examples PRIVATE 
	EXAMPLE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)