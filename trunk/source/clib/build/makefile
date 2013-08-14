#gnumake for this project.

include make_util.mak

src_path = test/src
obj_path = test/obj
bin_path = test/bin

target_name = test_exe
build_type  = debug

cc_exe      = $(CC)
cc_flags    =
cc_lib_path = 
ys_lib_path =

as_exe      =
as_flags    =

ar_exe      =
ar_flags    =

ld_exe      = 
ld_flags    =

$(info building '$(target_name)')

source_files := $(call grab_files_r,$(src_path),'*.c')
depend_files := $(patsubst $(src_path)/%,$(obj_path)/%,$(source_files:%.c=%.d))
object_files := $(patsubst $(src_path)/%,$(obj_path)/%,$(source_files:%.c=%.o))

$(info source files $(source_files))
$(info depend files $(depend_files))
$(info object files $(object_files))

$(call define_depend_variables,$(source_files))

$(foreach depend,$(source_files:%=%.d),$(info $(depend) = $($(depend))))
$(call dump_depend_variable,$(source_files),$(src_path),$(obj_path))

$(call gen_object_dependencies,'*.c',$(src_path),$(obj_path))

#$(info $(.VARIABLES))

$(target_name) : $(object_files)
	@echo [linking] $@
	@mkdir -p $(dir $@)
	@$(ld_exe) $(ld_flags) -o $@ $?

#$(target_name) : $(object_files)
#	@echo [archiving] $@
#	@mkdir -p $(dir $@)
#	@$(ar_exe) $(ar_flags) -o $@ $?

.PHONY: clean
clean:
	rm -dfr (obj_path)
	rm -dfr (bin_path)

#compile VCC files.
$(OBJ_PATH)/%.o: $(SOURCE_PATH)/%.vcc $(OBJ_PATH)/vc_compiler_extensions.pch
	@echo [VCC] $<
	@mkdir -p $(dir $@)
#	@echo [VCC PROCESSING] $< $(VCCFLAGS)
	@$(VCC) $(VCCFLAGS) +pp_out:$(basename $@).pp +precompiled:$(OBJ_PATH)/vc_compiler_extensions.pch +in:$(OBJ_PATH)/vc_compiler_extensions.h +in:$< +pp_out:$(basename $@).i +dep_out:$(basename $@).d +obj_out:$@ +cpp_out:$(basename $@).ii
#	@echo [GCC PROCESSING] $@ $(CPPFLAGS)
	@$(GCC) $(CPPFLAGS) $(basename $@).ii -o $@
#	@echo [GCC PROCESSING END]

#compile C++ files.
$(OBJ_PATH)/%.cpp_obj: $(SOURCE_PATH)/%.cpp
	@echo [C++] $< $@
	@mkdir -p $(dir $@)
	@$(GCC) $(CPPFLAGS) -MP -MD $(basename $@).d -o $@ $<

#compile C files.
$(OBJ_PATH)/%.c_obj: $(SOURCE_PATH)/%.c
	@echo [C] $< $@
	@mkdir -p $(dir $@)
	@$(GCC) $(CFLAGS) -MP -MD $(basename $@).d -o $@ $<

