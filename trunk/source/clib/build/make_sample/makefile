#gnumake for this project.

platform := OSX
include make_args
include make_util

src_path := test/src
obj_path := test/obj
bin_path := test/bin

target_name := test
target_path := $(bin_path)/lib$(target_name).a
build_type  := debug

$(info building '$(target_name)')
$(info ar_exe '$(ar_exe)' $(AR))

#$(target_path) : $(object_files)
#	@echo [linking] $@
#	@mkdir -p $(dir $@) && $(ld_exe) -o $@ $? $(ld_flags)

object_files := $(call gen_objectfiles,'*.c',$(src_path),$(obj_path))
$(target_path) : $(object_files)
	@echo [archiving] $@
	@mkdir -p $(dir $@) && $(ar_exe) $(ar_flags) $@ $?

# note gen_objectfiles_dependency should be locate after the first target, since this function will generate
# dependencies which would seize the first place
$(call gen_objectfiles_dependency,'*.c',$(src_path),$(obj_path))

.PHONY: clean
clean:
	rm -dfr $(obj_path)
	rm -dfr $(bin_path)
