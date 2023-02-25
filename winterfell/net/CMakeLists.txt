set(net_SRCS
  wf_event_loop.cc
  wf_channel.cc
  wf_poller.cc
  wf_timer.cc
  wf_timer_queue.cc
  poller/wf_poll_poller.cc
  poller/wf_epoll_poller.cc
  wf_endpoint.cc
  wf_acceptor.cc
  wf_sockets_ops.cc
  wf_socket.cc
  wf_tcp_server.cc
  wf_tcp_connection.cc
  wf_buffer.cc
  wf_sub_loops.cc
  wf_connector.cc
  wf_tcp_client.cc
)

add_library(winterfell_net ${net_SRCS})
target_link_libraries(winterfell_net winterfell_base winterfell_util pthread)

install(TARGETS winterfell_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/winterfell/net)

add_subdirectory(tests)
add_subdirectory(http)