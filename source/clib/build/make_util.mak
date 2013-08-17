#this file defines a series of functions make the working makefile job easier.

#$(call grab_files,$(src_paths),'*.c')
grab_files = $(foreach path,$(1),$(shell find $(path) -maxdepth 0 -type f -name $(2)))

#$(call grab_files_r, $(src_paths_recursive), '*.c')
grab_files_r = $(foreach path,$(1),$(shell find $(path) -type f -name $(2)))

#x = $(call gen_denpendency, src/main.c)
#x will be 'src/main.c src/include/a.h src/include/b.h src/include/c.h'
#sed -e 's/.*: //'               #strip the first target, since gcc -MM results in 'x.o: x.c a.h b.h c.h'
#sed -e 's/ *//'                 #strip the leading spaces
#sed -e 's/\\$//' | tr '\n' ' '  #replace \ and newline seperated files into space seperated. 
#gen_denpendency = $(shell $(cc_exe) -MM $(1) | sed -e 's/.*: //' -e 's/\\$//' -e 's/ *//' | tr '\n' ' ')
gen_denpendency = $(shell $(cc_exe) $(cc_flags) -MM $(cc_include_dir) $(1) | sed -e 's/.*: //' -e 's/\\$$//' -e 's/ *//' | tr '\n' ' ')

# $(call gen_objectfiles,$(src_pattern),$(src_path),$(obj_path))
gen_objectfiles = $(patsubst $(2)%,$(3)%.o,$(call grab_files_r,$(2),$(1)))

# $(call object_dependency_template,$(object_file_path),$(dependency))
define object_dependency_template
$(1): $(2)
	@echo [compiling] $$<
	@mkdir -p $$(dir $$@) && \
	$$(cc_exe) $$(cc_preprocess_ctrl) $$(cc_flags) $$(cc_include_dir) -o $$(basename $$@).i $$< && \
	$$(cc_exe) $$(cc_compile_ctrl) $$(cc_flags) -o $$(basename $$@).s $$(basename $$@).i && \
	$$(cc_exe) $$(cc_assemble_ctrl) $$(cc_flags) -o $$(basename $$@).o $$(basename $$@).s && \
	$$(cc_exe) $$(cc_dependency_ctrl) $$(cc_flags) $$(cc_include_dir) -o $$(basename $$@).d $$< 
endef

# $(call gen_objectfiles_dependency,$(src_pattern),$(src_path),$(obj_path))
# this function will generate a series of the object file dependency, in the above format.
# since this will actually generate dependencies, so this function should be called after 
# the main target of the makefile, or the 'first target' place will be occupied this call.
#
# this function should be call in pair of gen_objectfiles, please see the sample for more detail.
gen_objectfiles_dependency = $(foreach x,$(call grab_files_r,$(2),$(1)),$(eval $(call object_dependency_template,$(patsubst $(2)%,$(3)%.o,$(x)),$(call gen_denpendency,$(x)))))
