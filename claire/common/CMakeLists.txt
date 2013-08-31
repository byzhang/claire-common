set(common_SRCS
    logging/LogFile.cc
    logging/LogStream.cc
    logging/LogBuffer.cc
    logging/LogMessage.cc
    logging/Logger.cc
    logging/Logging.cc
    strings/StringPiece.cc
    strings/StringUtil.cc
    threading/Thread.cc
    threading/ThreadPool.cc
    threading/ThisThread.cc
    time/Timestamp.cc
    files/FileUtil.cc
    system/ProcessInfo.cc
    base/Exception.cc
    stats/Histogram.cc
    stats/Stats.cc
    stats/HistogramImpl.cc
  )

add_library(claire_common ${common_SRCS})
target_link_libraries(claire_common pthread rt gflags)

install(TARGETS claire_common DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/claire/common)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
