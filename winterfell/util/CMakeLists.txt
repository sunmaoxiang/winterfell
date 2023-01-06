
set(util_SRCS
  wf_util.cc
)

add_library(winterfell_util ${util_SRCS})
target_link_libraries(winterfell_util pthread)

install(TARGETS winterfell_util DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/winterfell/util)

