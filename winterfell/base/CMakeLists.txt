set(base_SRCS
  wf_log.cc
  wf_thread.cc
  wf_timestamp.cc
)

add_library(winterfell_base ${base_SRCS})
target_link_libraries(winterfell_base winterfell_util pthread)

install(TARGETS winterfell_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/winterfell/base)

add_subdirectory(tests)