add_custom_command(
	OUTPUT certificate.pem
	COMMAND 
	certtool --generate-privkey --outfile certificate.key &&
	certtool --generate-self-signed --load-privkey certificate.key --outfile certificate.crt --template ${CMAKE_CURRENT_SOURCE_DIR}/certificate.cfg  &&
	cat certificate.crt certificate.key > certificate.pem
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/certificate.cfg
)

add_executable(websocket main.c certificate.pem)
target_link_libraries(websocket talloc_visualization onion talloc-debug)