#add tests

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# BWT Test
add_executable(bwt_test bwt_test.cpp)
target_link_libraries(bwt_test gtest_main alx_bwt)

# Predecessor Test
add_executable(pred_test pred_test.cpp)
target_link_libraries(pred_test tdc gtest_main tdc)


# Wavelet Tree Pasta Test
add_executable(wt_pasta_test wt_pasta_test.cpp)
target_link_libraries(wt_pasta_test gtest_main -fopenmp pasta_wt)

# R-Index Test
add_executable(rindex_test rindex_test.cpp)
target_link_libraries(rindex_test gtest_main alx_rindex prezza_rindex subsampled_rindex)

# tdc::uint40_t Test
add_executable(uint40_test uint40_test.cpp)
target_include_directories(uint40_test PUBLIC ../include)
target_link_libraries(uint40_test gtest_main tdc malloc_count)

include(GoogleTest)
gtest_discover_tests(bwt_test)
gtest_discover_tests(pred_test)
gtest_discover_tests(wt_pasta_test)
gtest_discover_tests(rindex_test)
gtest_discover_tests(uint40_test)

