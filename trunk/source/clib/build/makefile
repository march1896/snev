#gnumake for this project.

platform := OSX
include make_args
include make_util
cc_include_dir = -I ../src/ -I ../src/cntr/ -I ../src/cntr2/ -I ../src/memheap/ -I ../src/util/
cc_include_dir += -I ../tests/ -I ../tests/cntr/ -I ../tests/cntr2/ -I ../tests/memheap/ -I ../tests/util/

clib_src_path := ../src
clib_obj_path := make/clib/obj
clib_bin_path := make/clib/bin
clib_target_name := clib
clib_target_path := $(clib_bin_path)/lib$(clib_target_name).a

test_src_path := ../tests
test_obj_path := make/test/obj
test_bin_path := make/test/bin

test_target_name := test
test_target_path := $(test_bin_path)/$(test_target_name).out
#build_type  := debug

$(info building '$(test_target_name)')
test_object_files := $(call gen_objectfiles,'*.c',$(test_src_path),$(test_obj_path))
$(test_target_path) : $(test_object_files) $(clib_target_path)
	@echo [linking] $@
	@mkdir -p $(dir $@) && $(ld_exe) -o $@ $^ $(ld_flags) $(clib_target_path)

$(call gen_objectfiles_dependency,'*.c',$(test_src_path),$(test_obj_path))

$(info building '$(clib_target_name)')
object_files := $(call gen_objectfiles,'*.c',$(clib_src_path),$(clib_obj_path))
$(clib_target_path) : $(object_files)
	@echo [archiving] $@
	@mkdir -p $(dir $@) && $(ar_exe) $(ar_flags) $@ $^

# note gen_objectfiles_dependency should be locate after the first target, since this function will generate
# dependencies which would seize the first place
$(call gen_objectfiles_dependency,'*.c',$(clib_src_path),$(clib_obj_path))

.PHONY: clean
clean:
	rm -dfr $(test_obj_path)
	rm -dfr $(test_bin_path)
	rm -dfr $(clib_obj_path)
	rm -dfr $(clib_bin_path)
