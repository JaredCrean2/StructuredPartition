message("Processing source directory")

file(GLOB src_files *.cc)


add_library(structured_partition "${src_files}")
target_include_directories(structured_partition PUBLIC
                          "${CMAKE_CURRENT_SOURCE_DIR}"
                          "${PROJECT_BINARY_DIR}/include"  # needed for configured header
)


set(ALL_LIBS ${ALL_LIBS} structured_partition PARENT_SCOPE)
message("ALL_LIBS = ${ALL_LIBS}")

#add_executable(structured_part main.cc)

#target_include_directories(structured_part PUBLIC
#                          "${PROJECT_BINARY_DIR}"
#                          "${PROJECT_SOURCE_DIR}/include"
#                          )
#target_link_libraries(heat_calc PUBLIC heat_lib)


#set(install_exes structured_part)
#install(TARGETS ${install_exes} DESTINATION bin)
