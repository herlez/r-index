cmake_minimum_required(VERSION 3.15)
project(r-index_seq)
set(CMAKE_CXX_STANDARD 20)


add_executable(bench benchmark.cpp)
target_compile_options(bench PRIVATE -Wall -Wextra -pedantic -O3 -DNDEBUG) #-Winline -save-temps #-Werror
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(bwt bench_bwt.cpp)
target_compile_options(bwt PRIVATE -Wall -Wextra -pedantic -O3) #-Winline -save-temps #-Werror
target_include_directories(bwt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(wt bench_wt.cpp)
target_compile_options(wt PRIVATE -Wall -Wextra -pedantic -O3) #-Winline -save-temps #-Werror
target_include_directories(wt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(gen_text generate_random_text.cpp)
target_compile_options(gen_text PRIVATE -Wall -Wextra -pedantic -O3) #-Winline -save-temps #-Werror
target_include_directories(gen_text PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(gen_stats generate_stats.cpp)
target_compile_options(gen_stats PRIVATE -Wall -Wextra -pedantic -O3) #-Winline -save-temps #-Werror
target_include_directories(gen_stats PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#include prezza's r-index
add_library(prezza_rindex INTERFACE)
target_include_directories(prezza_rindex INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/extlib/r-index/internal)
target_link_libraries(bench prezza_rindex)

#include libsais
add_library(libsais64
  extlib/libsais/src/libsais64.c
  extlib/libsais/src/libsais.c 
)
target_include_directories(libsais64 SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/libsais/src)
target_link_libraries(bench libsais64)
target_link_libraries(bwt libsais64)
target_link_libraries(wt libsais64)
target_link_libraries(gen_text libsais64)
target_link_libraries(gen_stats libsais64)



#include tdc
add_library(
  tdc
  extlib/tdc/src/vec/int_vector.cpp
  extlib/tdc/src/vec/allocate.cpp
) 
target_include_directories(tdc SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tdc/include)
target_link_libraries(bench tdc)
target_link_libraries(gen_stats tdc)

#include tlx
add_library(tlx
  extlib/tlx/tlx/cmdline_parser.cpp
  extlib/tlx/tlx/string/parse_si_iec_units.cpp
  extlib/tlx/tlx/die/core.cpp
)
target_include_directories(tlx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx/tlx
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/tlx)
target_link_libraries(bench tlx)
target_link_libraries(wt tlx)

#SDSL
target_include_directories(bench PUBLIC /usr/local/include)
target_link_directories(bench PUBLIC /usr/local/lib)
target_link_libraries(bench sdsl divsufsort divsufsort64)

#malloc_count
add_library(malloc_count ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count/malloc_count.c)
target_include_directories(malloc_count PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/malloc_count)

target_link_libraries(bench malloc_count -ldl)
target_link_libraries(bwt malloc_count -ldl)
target_link_libraries(wt malloc_count -ldl)
target_link_libraries(gen_stats malloc_count -ldl)

#include divsufsort
target_link_libraries(bench divsufsort64)
target_link_libraries(bwt divsufsort64)

#include pwm
target_include_directories(bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/extlib/pwm/include)
target_link_libraries(bench -fopenmp)

#include pasta
add_library(wt_pasta INTERFACE)
target_include_directories(wt_pasta 
SYSTEM INTERFACE extlib/wavelet_tree/
SYSTEM INTERFACE extlib/wavelet_tree/extlib/utils
SYSTEM INTERFACE extlib/wavelet_tree/extlib/bit_vector
SYSTEM INTERFACE extlib/tlx)

target_link_libraries(bench wt_pasta)
target_link_libraries(wt wt_pasta)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

