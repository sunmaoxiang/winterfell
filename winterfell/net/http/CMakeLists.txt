set(http_SRCS
  wf_http_server.cc
  wf_http_context.cc
  wf_http_response.cc
  )

add_library(winterfell_http ${http_SRCS})
target_link_libraries(winterfell_http winterfell_net)

install(TARGETS winterfell_http DESTINATION lib)
set(HEADERS
  wf_http_context.h
  wf_http_request.h
  wf_http_response.h
  wf_http_server.h
  )
install(FILES ${HEADERS} DESTINATION include/winterfell/net/http)

add_executable(httpserver_test tests/wf_http_server_test.cc)
target_link_libraries(httpserver_test winterfell_http)