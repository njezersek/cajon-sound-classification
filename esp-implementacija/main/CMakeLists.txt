file(GLOB SRCS "src/*.cpp")
file(GLOB ASETS_SRCS "assets/*.cpp")

idf_component_register(SRCS ${SRCS} ${ASETS_SRCS}
                    INCLUDE_DIRS "include" "assets")
