set(net_SRCS
  wf_event_loop.cc
  wf_channel.cc
  wf_poller.cc
  poller/wf_poll_poller.cc
  poller/wf_epoll_poller.cc
)

add_library(winterfell_net ${net_SRCS})
target_link_libraries(winterfell_net winterfell_base winterfell_util pthread)

install(TARGETS winterfell_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/winterfell/net)

add_subdirectory(tests)