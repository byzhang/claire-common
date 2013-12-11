add_custom_command(OUTPUT EscapeTables.cc
  COMMAND python
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/strings/generate_escape_tables.py
  VERBATIM )

set(common_SRCS
    logging/LogFile.cc
    logging/LogStream.cc
    logging/LogBuffer.cc
    logging/LogMessage.cc
    logging/Logger.cc
    logging/Logging.cc
    strings/StringPiece.cc
    strings/StringUtil.cc
    strings/UriEscape.cc
    strings/StringPrintf.cc
    threading/Mutex.cc
    threading/Thread.cc
    threading/ThreadPool.cc
    threading/ThisThread.cc
    time/Timestamp.cc
    files/FileUtil.cc
    system/ThisProcess.cc
    base/StackTrace.cc
    base/Exception.cc
    events/poller/EPollPoller.cc
    events/EventLoop.cc
    events/Channel.cc
    events/EventLoopThread.cc
    events/EventLoopThreadPool.cc
    events/TimeoutQueue.cc
    symbolizer/Elf.cc
    symbolizer/Dwarf.cc
    symbolizer/Symbolizer.cc
    EscapeTables.cc
    metrics/Counter.cc
    metrics/CounterProvider.cc
    metrics/CounterSampler.cc
    metrics/SampleVector.cc
    metrics/BucketRanges.cc
    metrics/HistogramSamples.cc
    metrics/HistogramRecorder.cc
    metrics/Histogram.cc
    protobuf/ProtobufIO.cc
  )

add_library(claire_common ${common_SRCS})
target_link_libraries(claire_common gflags protobuf pthread rt)

install(TARGETS claire_common DESTINATION lib)
file(GLOB LOGGING_HEADERS "logging/*.h")
install(FILES ${LOGGING_HEADERS} DESTINATION include/claire/common/logging/)

file(GLOB STRINGS_HEADERS "strings/*.h")
install(FILES ${STRINGS_HEADERS} DESTINATION include/claire/common/strings/)

file(GLOB THREADING_HEADERS "threading/*.h")
install(FILES ${THREADING_HEADERS} DESTINATION include/claire/common/threading/)

file(GLOB TIME_HEADERS "time/*.h")
install(FILES ${TIME_HEADERS} DESTINATION include/claire/common/time/)

file(GLOB FILES_HEADERS "files/*.h")
install(FILES ${FILES_HEADERS} DESTINATION include/claire/common/files/)

file(GLOB FILES_HEADERS "files/*.h")
install(FILES ${FILES_HEADERS} DESTINATION include/claire/common/files/)

file(GLOB BASE_HEADERS "base/*.h")
install(FILES ${BASE_HEADERS} DESTINATION include/claire/common/base/)

file(GLOB SYSTEM_HEADERS "system/*.h")
install(FILES ${SYSTEM_HEADERS} DESTINATION include/claire/common/system/)

file(GLOB EVENTLOOP_HEADERS "events/*.h")
install(FILES ${EVENTLOOP_HEADERS} DESTINATION include/claire/common/events/)

file(GLOB POLLER_HEADERS "events/poller/*.h")
install(FILES ${POLLER_HEADERS} DESTINATION include/claire/common/events/poller/)

file(GLOB SYMBOLIZER_HEADERS "symbolizer/*.h")
install(FILES ${SYMBOLIZER_HEADERS} DESTINATION include/claire/common/symbolizer/)

file(GLOB SYMBOLIZER_HEADERS "metrics/*.h")
install(FILES ${SYMBOLIZER_HEADERS} DESTINATION include/claire/common/metrics/)

file(GLOB PROTOBUF_HEADERS "protobuf/*.h")
install(FILES ${PROTOBUF_HEADERS} DESTINATION include/claire/common/protobuf/)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
