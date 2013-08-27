#gnumake for this project.

platform := CYGWIN
opt      := 0
debug    := 1

# TODO: we should add some debug/release related cc_flags in make_args.mak, according the below variable.
#build_type  := debug

# make_args.mak should be included prior to make_util.mak, since variables used in make_util.mak
# depends on make_arg.mak
include make_args.mak
include make_util.mak

src_root   := ../..
build_root := ./build
buildtype_folder := 
ifeq ($(debug), 0)
	buildtype_folder := release
else
	buildtype_folder := debug
endif

# TODO: remove so much include path.
cc_include_dir = -I $(src_root)/src/ -I $(src_root)/src/cntr/ -I $(src_root)/src/cntr2/ -I $(src_root)/src/memheap/ -I $(src_root)/src/util/
cc_include_dir += -I $(src_root)/tests/ -I $(src_root)/tests/cntr/ -I $(src_root)/tests/cntr2/ -I $(src_root)/tests/memheap/ -I $(src_root)/tests/util/

clib_src_path := $(src_root)/src
clib_obj_path := $(build_root)/clib/obj/$(buildtype_folder)
clib_bin_path := $(build_root)/clib/bin/$(buildtype_folder)
clib_target_name := clib
clib_target_path := $(clib_bin_path)/lib/$(clib_target_name).a

test_src_path := $(src_root)/tests
test_obj_path := $(build_root)/test/obj/$(buildtype_folder)
test_bin_path := $(build_root)/test/bin/$(buildtype_folder)

test_target_name := test
test_target_path := $(test_bin_path)/$(test_target_name).out

#$(info building '$(test_target_name)')
test_object_files := $(call gen_objectfiles,'*.c',$(test_src_path),$(test_obj_path))
$(test_target_path) : $(test_object_files) $(clib_target_path)
	@echo [linking] $@
	@mkdir -p $(dir $@) && $(ld_exe) $(ld_flags_front) -o $@ $^ $(clib_target_path) $(ld_flags_back)

$(call gen_objectfiles_dependency,'*.c',$(test_src_path),$(test_obj_path))

#$(info building '$(clib_target_name)')
object_files := $(call gen_objectfiles,'*.c',$(clib_src_path),$(clib_obj_path))
$(clib_target_path) : $(object_files)
	@echo [archiving] $@
	@mkdir -p $(dir $@) && $(ar_exe) $(ar_flags) $@ $^

# note gen_objectfiles_dependency should be locate after the first target, since this function will generate
# dependencies which would seize the first place
$(call gen_objectfiles_dependency,'*.c',$(clib_src_path),$(clib_obj_path))

.PHONY: clean
clean:
	@echo [cleaning] $(clib_target_name)
	@rm -fr $(clib_obj_path)
	@rm -fr $(clib_bin_path)
	@echo [cleaning] $(test_target_name)
	@rm -fr $(test_obj_path)
	@rm -fr $(test_bin_path)
	@rm -fr $(out_root)
