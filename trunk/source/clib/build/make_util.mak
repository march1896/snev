# this file defines a series of functions make the working makefile job easier.

# $(call grab_files $(src_paths) '*.c')
grab_files = $(foreach path, $(1), $(shell find $(path) -maxdepth 0 -type f -name $(2)))

# $(call grab_files_r $(src_paths_recursive) '*.c')
grab_files_r = $(foreach path, $(1), $(shell find $(path) -type f -name $(2)))

# 
gen_denpendency 
