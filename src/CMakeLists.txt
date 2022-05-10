include_directories(${Graphics_Engine_SOURCE_DIR}/include)
set(SOURCE_FILES main.cpp)

if(WIN32)
    add_executable(Graphics_Engine WIN32 ${SOURCE_FILES})
else()
    add_executable(Graphics_Engine ${SOURCE_FILES})
endif()

set(SFML_STATIC_LIBRARIES TRUE)
target_include_directories(${PROJECT_NAME} PUBLIC include)

#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED COMPONENTS graphics window audio network system)
target_link_libraries(Graphics_Engine sfml-graphics sfml-window sfml-audio sfml-network sfml-system)
#target_link_libraries(SFML_project -static)

install(TARGETS Graphics_Engine DESTINATION ${CMAKE_BINARY_DIR}/build)
