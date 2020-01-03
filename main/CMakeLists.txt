idf_component_register(SRCS "main.c" "server.c" "connect.c" "stdin_out.c" "moonlight.c"
                    INCLUDE_DIRS ".")

if(CONFIG_WEB_DEPLOY_SF)
    set(FRONT_END "${CMAKE_CURRENT_SOURCE_DIR}/../frontend")
    if(EXISTS ${FRONT_END})
        spiffs_create_partition_image(www ${FRONT_END} FLASH_IN_PROJECT)
    else()
        message(FATAL_ERROR "${FRONT_END} doesn't exit.")
    endif()
endif()
