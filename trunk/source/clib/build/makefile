#gnumake for this project.

platform := OSX
include make_args
include make_util

src_path = test/src
obj_path = test/obj
bin_path = test/bin

target_name = test_exe
build_type  = debug

$(info building '$(target_name)')

#$(info $(.VARIABLES))

object_files = $(call gen_objectfiles,'*.c',$(src_path),$(obj_path))
$(info $(object_files))
$(bin_path)/$(target_name) : $(object_files)
	@echo [linking] $@
	@mkdir -p $(dir $@) && $(ld_exe) -o $@ $? $(ld_flags)

$(call gen_objectfiles_dependency,'*.c',$(src_path),$(obj_path))

#$(target_name) : $(object_files)
#	@echo [archiving] $@
#	@mkdir -p $(dir $@)
#	@$(ar_exe) $(ar_flags) -o $@ $?

.PHONY: clean
clean:
	rm -dfr $(obj_path)
	rm -dfr $(bin_path)
