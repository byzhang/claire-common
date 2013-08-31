add_executable(logger_test Logger_test.cc)
target_link_libraries(logger_test claire_common)

add_executable(counter_test Counter_test.cc)
target_link_libraries(counter_test claire_common)

add_executable(metric_test Metric_test.cc)
target_link_libraries(metric_test claire_common)

add_executable(gauge_test Gauge_test.cc)
target_link_libraries(gauge_test claire_common)

add_executable(histogram_unitteest Histogram_unittest.cc)
target_link_libraries(histogram_unitteest claire_common gtest gtest_main)
