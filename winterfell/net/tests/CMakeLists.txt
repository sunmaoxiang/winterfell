set(targets wf_tcp_client_test; 
    wf_sub_loops_test;
    wf_buffer_test;
    wf_event_loop_test1;
    wf_event_loop_test2;
    wf_event_loop_test3;
    wf_acceptor_test;
    wf_tcp_server_test;
    wf_endpoint_test;
    wf_timer_queue_test;)

foreach(X IN LISTS targets) 
  add_executable(${X} ${X}.cc) 
  target_link_libraries(${X} winterfell_net winterfell_base winterfell_util gtest)
endforeach()