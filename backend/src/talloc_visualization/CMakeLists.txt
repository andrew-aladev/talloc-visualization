set (SOURCES  socket.c epoll.c process.c)
set (INCLUDES socket.h epoll.h process.h)

add_subdirectory (utils)

add_library (${PROJECT_NAME} ${SOURCES})
target_link_libraries (${PROJECT_NAME} wslay talloc2)