find_package(Ruby)

if(NOT RUBY_EXECUTABLE)
    message(STATUS "mruby -> OFF")
    return()
endif()

get_filename_component(ruby_dir "${RUBY_EXECUTABLE}" DIRECTORY)
find_file(rake_script NAMES rake PATHS "${ruby_dir}")
if(NOT rake_script)
    execute_process(COMMAND "${RUBY_EXECUTABLE}" "${ruby_dir}/gem" install rake)
    find_file(rake_script NAMES rake PATHS "${ruby_dir}")
    if(NOT rake_script)
        message(WARNING "Failed: gem install rake")
        message(STATUS "mruby -> OFF")
        return()
    endif()
endif()

FetchContent_Declare(mruby DOWNLOAD_EXTRACT_TIMESTAMP ON
    GIT_REPOSITORY "https://github.com/mruby/mruby.git"
    GIT_TAG "3.2.0"
    UPDATE_COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/gfx2agb.rb" <SOURCE_DIR>/build_config
)

FetchContent_MakeAvailable(mruby)

set(mruby_library "${mruby_SOURCE_DIR}/build/host/lib/libmruby${CMAKE_STATIC_LIBRARY_SUFFIX}")
add_custom_command(
    OUTPUT "${mruby_library}"
    COMMAND "${CMAKE_COMMAND}" -E env MRUBY_CONFIG=gfx2agb -- "${RUBY_EXECUTABLE}" "${rake_script}"
    WORKING_DIRECTORY "${mruby_SOURCE_DIR}"
)

add_custom_target(configure_mruby DEPENDS "${mruby_library}")

add_library(mruby STATIC IMPORTED)
set_property(TARGET mruby PROPERTY IMPORTED_LOCATION "${mruby_library}")
target_include_directories(mruby INTERFACE "${mruby_SOURCE_DIR}/include")
add_dependencies(mruby configure_mruby)
