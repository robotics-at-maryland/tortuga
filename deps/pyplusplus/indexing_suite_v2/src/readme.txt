Installation tip:
    1. copy "indexing" directory to ".../libs/python/src" directory\
    2. add to ".../libs/python/build/Jamfile[.v2]" next lines:
       
       indexing/indexing_slice.cpp
       indexing/python_iterator.cpp
      
       You should add those lines to the list of source files.