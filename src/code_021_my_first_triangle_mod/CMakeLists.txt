add_executable(code_021_my_first_triangle_mod main_021.cpp)

find_package(OpenGL)


target_include_directories(code_021_my_first_triangle_mod PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}        # anchors relative includes here
)


target_link_libraries(code_021_my_first_triangle_mod
    PRIVATE
        glfw
        glad
        OpenGL::GL
)
