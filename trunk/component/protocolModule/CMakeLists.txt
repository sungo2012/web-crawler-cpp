aux_source_directory(. DIR_HTTP)
add_library(HttpModule ${DIR_HTTP})
target_link_libraries(HttpModule pthread TinyDNSResolver rio)
