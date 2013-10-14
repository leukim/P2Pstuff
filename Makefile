all: main.c T-110_5150_reference_c_header.h helpers.h structures.h core.h core_helpers.h data.h
	gcc -o main main.c
