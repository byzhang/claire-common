add_executable(logger_test Logger_test.cc)
target_link_libraries(logger_test claire_common)

add_executable(counter_test Counter_test.cc)
target_link_libraries(counter_test claire_common)

add_executable(metric_test Metric_test.cc)
target_link_libraries(metric_test claire_common)


