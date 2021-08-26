if(ENABLE_TESTING)
  add_subdirectory(third_party/catch2)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/third_party/catch2/contrib")
  enable_testing()
  include(Catch)

  macro(native_test name)
    add_executable(test_native_${name} "${CMAKE_SOURCE_DIR}/test/test_native_${name}.cxx")
    target_include_directories(test_native_${name} PRIVATE ${PROJECT_BINARY_DIR}/generated)
    target_link_libraries(
      test_native_${name}
      project_options
      project_warnings
      Catch2::Catch2
      OpenSSL::SSL
      OpenSSL::Crypto
      platform
      cbcrypto
      cbsasl
      http_parser
      hdr_histogram_static
      snappy
      spdlog::spdlog_header_only)
    catch_discover_tests(test_native_${name})
  endmacro()

  add_subdirectory(${CMAKE_SOURCE_DIR}/test)
endif()