add_executable(superpico-digital
    main.c
    video/video_pipeline.c
    video/video_capture.c
    video/freq_counter.c
    audio/audio_pipeline.c
    osd/osd.c
)

# Add pico_hdmi library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/pico_hdmi ${CMAKE_CURRENT_BINARY_DIR}/pico_hdmi)

pico_generate_pio_header(superpico-digital ${CMAKE_CURRENT_LIST_DIR}/video/video_capture.pio)

target_include_directories(superpico-digital PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/video
    ${CMAKE_CURRENT_LIST_DIR}/audio
    ${CMAKE_CURRENT_LIST_DIR}/osd
)

target_link_libraries(superpico-digital
    pico_hdmi
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_clocks
    hardware_dma
    hardware_irq
    hardware_gpio
    hardware_interp
)

pico_enable_stdio_usb(superpico-digital 1)
pico_enable_stdio_uart(superpico-digital 0)
pico_add_extra_outputs(superpico-digital)
