idf_component_register(
    SRCS
        "main.cpp"
        "../ui/performance_screen.cpp"
        "../../../src/ui/tab5_ui.cpp"
        "../../../src/ui/tab5_touch.cpp"
    INCLUDE_DIRS
        "."
        "../ui"
        "../../../src"
    REQUIRES
        m5unified
        m5gfx
)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_17)
