add_executable(superpico-digital
    main.c
    settings.c
    video/video_pipeline.c
    video/video_capture.c
    video/freq_counter.c
    audio/audio_pipeline.c
    audio/i2s_capture.c
    audio/audio_buffer.c
    audio/src.c
    osd/fast_osd.c
    osd/selftest_layout.c
    experiments/menu_diag_experiment.c
)

option(SUPERPICO_COPY_TO_RAM
    "Copy the firmware image to SRAM at boot and execute from RAM, matching the NeoPico-HD timing-isolation mode"
    ON)

# Match NeoPico-HD's HDMI backend: one RT build can boot into 480p, 240p, or
# 720p via the resolution menu. 480p uses a 252 MHz sysclk with HSTX div2 so
# the output pixel clock remains the normal 25.2 MHz signal.
set(PICO_HDMI_RUNTIME_MODES ON CACHE BOOL "" FORCE)
set(PICO_HDMI_PRECOMPOSED_ACTIVE_LINES OFF CACHE BOOL "" FORCE)
set(PICO_HDMI_RT_RUNTIME_MODE_ATTRS ON CACHE BOOL "" FORCE)

# Add pico_hdmi library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lib/pico_hdmi ${CMAKE_CURRENT_BINARY_DIR}/pico_hdmi)

target_compile_definitions(pico_hdmi PRIVATE
    PICO_HDMI_480P_HSTX_CLK_DIV=2
    PICO_HDMI_LEGACY_240P_AVI_INFOFRAME=1
)

pico_generate_pio_header(superpico-digital ${CMAKE_CURRENT_LIST_DIR}/video/video_capture.pio)
pico_generate_pio_header(superpico-digital ${CMAKE_CURRENT_LIST_DIR}/audio/i2s_capture.pio)

target_include_directories(superpico-digital PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/video
    ${CMAKE_CURRENT_LIST_DIR}/audio
    ${CMAKE_CURRENT_LIST_DIR}/osd
    ${CMAKE_CURRENT_LIST_DIR}/experiments
)

target_link_libraries(superpico-digital
    pico_hdmi
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_clocks
    hardware_dma
    hardware_flash
    hardware_irq
    hardware_gpio
    hardware_interp
    hardware_vreg
)

if(SUPERPICO_COPY_TO_RAM)
    pico_set_binary_type(superpico-digital copy_to_ram)
endif()

pico_enable_stdio_usb(superpico-digital 1)
pico_enable_stdio_uart(superpico-digital 0)
pico_add_extra_outputs(superpico-digital)
