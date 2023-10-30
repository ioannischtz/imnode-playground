add_executable(main_app main.cpp)
target_link_libraries(main_app PUBLIC imgui imgui-glfw imgui-opengl3 imnodes)
target_include_directories(main_app PUBLIC ../ext/imnodes)
target_include_directories(main_app PUBLIC ../include)
