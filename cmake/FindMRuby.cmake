FetchContent_Declare(mruby_proj DOWNLOAD_EXTRACT_TIMESTAMP ON
    GIT_REPOSITORY "https://github.com/mruby/mruby.git"
    GIT_TAG "3.2.0"
)

FetchContent_GetProperties(mruby_proj)
if(NOT mruby_proj_POPULATED)
    FetchContent_Populate(mruby_proj)
endif()

enable_language(C)
file(GLOB_RECURSE mrubysrc "${mruby_proj_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE mrubycompiler "${mruby_proj_SOURCE_DIR}/mrbgems/mruby-compiler/core/*.c" "${mruby_proj_SOURCE_DIR}/mrbgems/mruby-compiler/core/*.y")

add_library(mruby STATIC ${mrubysrc} ${mrubycompiler})
target_compile_definitions(mruby PRIVATE MRB_NO_PRESYM)
target_include_directories(mruby PUBLIC "${mruby_proj_SOURCE_DIR}/include")
