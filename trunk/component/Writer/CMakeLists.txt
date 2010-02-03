aux_source_directory(. DIR_WRITER)
	add_library(Writer ${DIR_WRITER})
target_link_libraries(Writer ris)
