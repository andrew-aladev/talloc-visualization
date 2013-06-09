set (SOURCES  socket.c epoll.c process.c handshake.c events.c)
set (INCLUDES socket.h epoll.h process.h handshake.h events.h)

add_subdirectory (utils)

add_library (${PROJECT_NAME} ${SOURCES})
target_link_libraries (${PROJECT_NAME} wslay libbtr talloc2 gnutls)