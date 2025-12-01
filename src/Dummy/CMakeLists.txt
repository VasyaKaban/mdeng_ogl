add_executable(dummy)

target_sources(
dummy
    PRIVATE
        main.cpp
)

target_include_directories(dummy PUBLIC "../")

target_link_libraries(dummy
						PUBLIC Core
					)