# Gather all .cpp files in src and its subdirs
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS *.cpp)

add_library(blam STATIC ${SRC_FILES})
target_include_directories(blam PUBLIC ../include)
