add_cmake_project(Boost
    URL "https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.zip"
    URL_HASH SHA256=68cf5e488c9e6315ee101460b216cc0da6a401e527ff53446122d8c047a8fab3
    LIST_SEPARATOR |
    CMAKE_ARGS
        -DBOOST_INCLUDE_LIBRARIES:STRING=beast|nowide
        -DBOOST_EXCLUDE_LIBRARIES:STRING=context|coroutine
        -DBUILD_TESTING:BOOL=OFF
)