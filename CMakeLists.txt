PROJECT(crawler)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)

ADD_SUBDIRECTORY( component/ContentSeen )

ADD_SUBDIRECTORY( component/LinkExtractor )

ADD_SUBDIRECTORY( component/ris )

ADD_SUBDIRECTORY( component/URLFrontier )

ADD_SUBDIRECTORY( component/Writer )

ADD_SUBDIRECTORY( component/DNS_Resolver )

ADD_SUBDIRECTORY( component/protocolModule )

ADD_SUBDIRECTORY( component/URLFilter )

ADD_SUBDIRECTORY( component/URLSeen )

ADD_SUBDIRECTORY( util/btree-f )

ADD_SUBDIRECTORY( util/btree-m )

ADD_SUBDIRECTORY( util/lru )

ADD_SUBDIRECTORY( util/RabinHash )

ADD_SUBDIRECTORY( util/rio )

ADD_SUBDIRECTORY( util/rwlock_fcfs )

ADD_SUBDIRECTORY( util/thread_print )



SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O2 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")

ADD_EXECUTABLE(crawler ${DIR_SRCS})

TARGET_LINK_LIBRARIES(crawler pthread ContentSeen LinkExtractor ris
		URLFrontier Writer TinyDNSResolver HttpModule URLFilter
		URLSeen BTTableClass Basic_LRU env RabinHashFunction64
		Thread_print rio)
