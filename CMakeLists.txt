cmake_minimum_required(VERSION 3.15)
project(r-index_seq)
set(CMAKE_CXX_STANDARD 20)

add_executable(bench benchmark.cpp)
target_compile_options(bench PRIVATE -Wall -Wextra -pedantic -O3 -DNDEBUG) #-Winline -save-temps #-Werror
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(bwt bench_bwt.cpp)
target_compile_options(bwt PRIVATE -Wall -Wextra -pedantic -O3) #-Winline -save-temps #-Werror
target_include_directories(bwt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


#include prezza's r-index
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/r-index/internal)

#include libsais
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/libsais/src)
target_include_directories(bwt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/libsais/src)

add_library(libsais64
  extlib/libsais/src/libsais64.c
  extlib/libsais/src/libsais.c 
)
target_include_directories(libsais64 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/r-index/internal)
target_link_libraries(bench libsais64)
target_link_libraries(bwt libsais64)


#include tlx
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx/tlx)
add_library(tlx
  extlib/tlx/tlx/cmdline_parser.cpp
  extlib/tlx/tlx/string/parse_si_iec_units.cpp
  extlib/tlx/tlx/die/core.cpp
)
target_include_directories(tlx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx/)
target_link_libraries(bench tlx)


#SDSL
target_include_directories(bench PUBLIC /usr/local/include)
target_link_directories(bench PUBLIC /usr/local/lib)
target_link_libraries(bench sdsl divsufsort divsufsort64)

#malloc_count
add_library(malloc_count ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count/malloc_count.c)
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count)
target_link_libraries(bench malloc_count -ldl)
target_include_directories(bwt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count)
target_link_libraries(bwt malloc_count -ldl)

#include divsufsort
target_link_libraries(bench divsufsort64)
target_link_libraries(bwt divsufsort64)

#include pwm
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/pwm/include)
target_link_libraries(bench -fopenmp)


