#this file defines a series of functions make the working makefile job easier.

#$(call grab_files,$(src_paths),'*.c')
grab_files = $(foreach path,$(1),$(shell find $(path) -maxdepth 0 -type f -name $(2)))

#$(call grab_files_r, $(src_paths_recursive), '*.c')
grab_files_r = $(foreach path,$(1),$(shell find $(path) -type f -name $(2)))

#x = $(call gen_denpendency, src/main.c)
#x will be 'a.h b.h c.h'
#sed -e 's/.*: //'               #strip the first target, since gcc -MM results in 'x.o: x.c a.h b.h c.h'
#sed -e 's/ *//'                 #strip the leading spaces
#sed -e 's/\\$//' | tr '\n' ' '  #replace \ and newline seperated files into space seperated. 
#gen_denpendency = $(shell $(cc_exe) -MM $(1) | sed -e 's/.*: //' -e 's/\\$//' -e 's/ *//' | tr '\n' ' ')
gen_denpendency = $(shell $(cc_exe) -MM $(1) | sed -e 's/.*: //' -e 's/\\$$//' -e 's/ *//' | tr '\n' ' ')

# create a new variable
define assign_variable
  $(1) := $(2)
endef 

# $(call define_depend_variables,$(src_files))
# suppose you have src_files = 'a.c b.c dir/c dir/d.c'
# by call this function, there will be a series of variable, 'a.d b.d dir/c.d dir/d.d' which holds the 
# dependencies of the source file, that is:
# a.c.d = a.c a.h b.h balabala
# b.c.d = b.c b.h c.h balabala
# dir/c.c.d = dir/c.c dir/c.h balabala # note 'dir/c.d' is just the variable name
# dir/d.c.d = dir/d.d dir/d.h balabala 
# you can dump the details by the following function
# define_depend_variables = $(foreach x,$(l),$(info $(call assign_variable,$(x),$(call gen_denpendency,$(x)))))
define_depend_variables = $(foreach x,$(1),$(eval $(call assign_variable,$(patsubst %,%.d,$(x)),$(call gen_denpendency,$(x)))))

# $(call force_redirect,$(file_path),$(content))
# force_redirect = $(info mkdir -p $(dir $(1)) && echo $(2) > $(1))
force_redirect = $(shell mkdir -p $(dir $(1)) && echo $(2) > $(1))

# $(call dump_depend_variable,$(src_files),$(src_path),$(dump_path))
# dump_depend_variable = $(foreach x,$(1),$(shell echo $(patsubst %,%.d,$(x)) > $(patsubst $(2)%,$(3)%.d,$(x))))
dump_depend_variable = $(foreach x,$(1),$(call force_redirect,$(patsubst $(2)%,$(3)%.d,$(x)),$($(patsubst %,%.d,$(x)))))


# $(call gen_object_files,$(src_pattern),$(src_path),$(obj_path))

# $(call object_dependency_template,$(object_file_path),$(dependency))
define object_dependency_template
$(1): $(2)
  $(cc_exe) $(cc_flags) -o $@ $<
  $(info $(1) $(2))
endef

# we should generate preprocessed file and dependency file here.
define object_dependency_template_v
$(1): $(2)
  $(cc_exe) $(cc_flags) -o $@ $<
endef

# $(call gen_object_dependencies,$(src_pattern),$(src_path),$(obj_path))
#gen_object_dependencies = $(foreach x,$(call grab_files_r,$(2),$(1)),$(info it's me $(x)))
gen_object_dependencies = $(foreach x,$(call grab_files_r,$(2),$(1)),$(eval $(call object_dependency_template,$(patsubst $(2)%,$(3)%,$(x)),$(call gen_denpendency,$(x)))))
