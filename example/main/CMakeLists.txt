idf_component_register(SRCS
		"uart_example.c"
		INCLUDE_DIRS ".")

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_LVGL_H_INCLUDE_SIMPLE")
