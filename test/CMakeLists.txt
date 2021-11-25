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

# Predecessor Test
add_executable(pred_test pred_test.cpp
  ../extlib/tdc/src/pred/index.cpp
  ../extlib/tdc/src/vec/int_vector.cpp
  ../extlib/tdc/src/vec/allocate.cpp
)
target_include_directories(pred_test PUBLIC ../extlib/tdc/include)
target_link_libraries(pred_test  gtest_main)


add_executable(wt_test wt_test.cpp)
target_compile_options(wt_test PRIVATE -DNDEBUG) ##REMOVE LATER
target_include_directories(wt_test PUBLIC ../extlib/pwm/include)
target_link_libraries(wt_test gtest_main -fopenmp)





include(GoogleTest)
gtest_discover_tests(pred_test)
gtest_discover_tests(wt_test)

