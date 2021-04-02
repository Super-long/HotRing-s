cmake_minimum_required(VERSION 3.4.1)
project(HotRing)

set(CMAKE_CXX_STANDARD 17)
#link_libraries(-ltcmalloc)      # 用于tcmalloc

SET(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} -g -fno-strict-aliasing -O2 -Wall -Wextra -pipe
-D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 -Wnonnull -Wstringop-overflow -Wuninitialized -Wunused 
-fno-threadsafe-statics #可能会用到局部static，但是不希望其线程安全，这样可以使得代码生成的更少
)

aux_source_directory(src HOTRING)

add_executable(running_ht
    update_delete.cpp
    ${HOTRING}
)