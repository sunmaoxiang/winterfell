add_executable(wf_log_test wf_log_test.cc)
target_link_libraries(wf_log_test winterfell_base winterfell_util)

add_executable(wf_thread_test wf_thread_test.cc)
target_link_libraries(wf_thread_test winterfell_base winterfell_util)

add_executable(wf_timestamp_test wf_timestamp_test.cc)
target_link_libraries(wf_timestamp_test winterfell_base winterfell_util)

add_executable(wf_synchronization_test wf_synchronization_test.cc)
target_link_libraries(wf_synchronization_test winterfell_base winterfell_util)

add_executable(wf_rate_test wf_rate_test.cc)
target_link_libraries(wf_rate_test winterfell_base winterfell_util)