cmake_minimum_required(VERSION 3.21.0...3.29)
project(TND004-Lab-3-part2 VERSION 1.0.0 DESCRIPTION "TND004 Lab 3: part2" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab3-part2	
    #include/linesdiscoverysystem/file1.h	 # ADD other header files, if needed
	include/linesdiscoverysystem/readfiles.h 
    include/rendering/window.h  
    #src/linesdiscoverysystem/file1.cpp		#ADD other source files, if needed
	src/linesdiscoverysystem/readfiles.cpp 
    src/rendering/window.cpp
	src/lab3-part2.cpp	# main   
)

target_include_directories(lab3-part2 PUBLIC "include")
target_compile_options(lab3-part2 PUBLIC 
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra>
)

# External libraries
find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
target_link_libraries(lab3-part2 PUBLIC glm::glm fmt::fmt glad::glad glfw)
target_compile_definitions(lab3-part2 PRIVATE DATA_DIR=\"${CMAKE_CURRENT_LIST_DIR}/data\")