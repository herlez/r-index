cmake_minimum_required(VERSION 3.15)
project(r-index_seq)
set(CMAKE_CXX_STANDARD 20)


#include libsais
add_library(libsais64
  extlib/libsais/src/libsais64.c
  extlib/libsais/src/libsais.c 
)
target_include_directories(libsais64 SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/libsais/src)

#include alx_bwt
add_library(alx_bwt INTERFACE)
target_include_directories(alx_bwt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(alx_bwt INTERFACE libsais64)

#include tdc
add_library(tdc
  extlib/tdc/src/vec/int_vector.cpp
  extlib/tdc/src/vec/allocate.cpp
)
target_include_directories(tdc SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tdc/include)

#include pasta_wt
add_library(pasta_wt INTERFACE)
target_include_directories(pasta_wt 
SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/wavelet_tree/
SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/wavelet_tree/extlib/utils
SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/wavelet_tree/extlib/bit_vector
SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx)

add_library(pwm INTERFACE)
target_include_directories(pwm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/pwm/include)

#include prezza's r-index
add_library(prezza_rindex INTERFACE)
target_include_directories(prezza_rindex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/r-index/internal
INTERFACE /usr/local/include)
target_link_directories(prezza_rindex INTERFACE /usr/local/lib)
target_link_libraries(prezza_rindex INTERFACE sdsl divsufsort divsufsort64)

add_library(alx_rindex INTERFACE)
target_link_libraries(alx_rindex INTERFACE alx_bwt tdc pasta_wt malloc_count)

#include malloc_count
add_library(malloc_count ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count/malloc_count.c)
target_include_directories(malloc_count PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count)
target_link_libraries(malloc_count -ldl)

#include tlx
add_library(tlx
  extlib/tlx/tlx/cmdline_parser.cpp
  extlib/tlx/tlx/string/parse_si_iec_units.cpp
  extlib/tlx/tlx/die/core.cpp
)
target_include_directories(tlx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx/tlx
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx)

#EXECUTABLES
add_executable(bench benchmark.cpp)
target_compile_options(bench PRIVATE -Wall -Wextra -pedantic -O3 -DNDEBUG)
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bench prezza_rindex alx_rindex tlx malloc_count)

add_executable(bwt bench_bwt.cpp)
target_compile_options(bwt PRIVATE -Wall -Wextra -pedantic -O3)
target_include_directories(bwt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bwt alx_bwt divsufsort64 malloc_count)

add_executable(wt bench_wt.cpp)
target_compile_options(wt PRIVATE -Wall -Wextra -pedantic -O3)
target_include_directories(wt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(wt pasta_wt malloc_count)

add_executable(gen_text generate_random_text.cpp)
target_compile_options(gen_text PRIVATE -Wall -Wextra -pedantic -O3)
target_include_directories(gen_text PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(gen_stats generate_stats.cpp)
target_compile_options(gen_stats PRIVATE -Wall -Wextra -pedantic -O3)
target_include_directories(gen_stats PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(gen_stats malloc_count)

add_executable(gen_structs generate_structures.cpp)
target_compile_options(gen_structs PRIVATE -Wall -Wextra -pedantic -O3)
target_include_directories(gen_structs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(gen_structs libsais64 tdc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

