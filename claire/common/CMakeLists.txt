set(common_SRCS
    logging/LogFile.cc
    logging/LogStream.cc
    logging/LogBuffer.cc
    logging/LogMessage.cc
    logging/Logger.cc
    logging/Logging.cc
    string/StringPiece.cc
    string/StringUtil.cc
    threading/Thread.cc
    threading/ThreadPool.cc
    threading/ThisThread.cc
    time/Timestamp.cc
    file/FileUtil.cc
    system/ProcessInfo.cc
    Exception.cc
  )

add_library(claire_common ${common_SRCS})
target_link_libraries(claire_common pthread rt gflags)

install(TARGETS claire_common DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/claire/common)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
