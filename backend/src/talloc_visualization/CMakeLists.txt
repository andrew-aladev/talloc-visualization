set (SOURCES  socket.c epoll.c process.c handshake.c events.c talloc_events.c timer.c)
set (INCLUDES socket.h epoll.h process.h handshake.h events.h talloc_events.h timer.h)

add_subdirectory (utils)

add_library (${PROJECT_NAME} ${SOURCES})
target_link_libraries (${PROJECT_NAME} wslay libbtr talloc2 gnutls)