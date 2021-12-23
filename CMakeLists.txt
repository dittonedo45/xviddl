project(dnxvid)
add_executable(dnxvid routines.c vidx.c)
target_link_libraries(dnxvid PUBLIC "-I. -lxml2 -ljansson -latomic -lavformat")
