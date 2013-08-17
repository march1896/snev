#gnumake for this project.

platform := CYGWIN
include ../make_args.mak
include ../make_util.mak

src_path := ../src
obj_path := obj
bin_path := bin

target_name := test
target_path := $(bin_path)/$(target_name).out
build_type  := debug

object_files := $(call gen_objectfiles,'*.c',$(src_path),$(obj_path))
$(target_path) : $(object_files)
	@echo [linking] $@
	@mkdir -p $(dir $@) && $(ld_exe) $(ld_flags_front) -o $@ $^ $(ld_flags_back)

#object_files := $(call gen_objectfiles,'*.c',$(src_path),$(obj_path))
#$(target_path) : $(object_files)
#@echo [archiving] $@
#@mkdir -p $(dir $@) && $(ar_exe) $(ar_flags) $@ $^

# note gen_objectfiles_dependency should be locate after the first target, since this function will generate
# dependencies which would seize the first place
$(call gen_objectfiles_dependency,'*.c',$(src_path),$(obj_path))

.PHONY: clean
clean:
	@echo [cleaning] $(target_name)
	@rm -fr $(obj_path)
	@rm -fr $(bin_path)
