target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CONFIG_MY_DRIVER)
  target_sources(app PRIVATE my_driver.c)
endif ()