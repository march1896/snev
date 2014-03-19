ifeq ($(platform),OSX)
# $(CC) on OSX default points llvm-gcc.
cc_exe             := $(CC)
cc_flags           :=
cc_dependency_ctrl := -MM
cc_preprocess_ctrl := -E
cc_compile_ctrl    := -S
cc_assemble_ctrl   := -c

ifeq ($(opt), 0)
	cc_compile_ctrl += -O0
endif
ifeq ($(opt), 1)
	cc_compile_ctrl += -O1
endif
ifeq ($(opt), 2)
	cc_compile_ctrl += -O2
endif
ifeq ($(opt), 3)
	cc_compile_ctrl += -O3
endif
ifeq ($(debug), 1)
	cc_compile_ctrl += -g
endif

ar_exe             := $(AR)
ar_flags           := cr

ld_exe             := $(LD)
ld_exe             := $(CC)
ld_flags_front     :=
ld_flags_back      := -demangle -dynamic -arch x86_64 -macosx_version_min 10.8.0 -lSystem /usr/bin/../lib/clang/4.2/lib/darwin/libclang_rt.osx.a
ld_flags_back      :=

endif

ifeq ($(platform),CYGWIN)

cc_exe             := $(CC)
cc_flags           :=
cc_dependency_ctrl := -MM
cc_preprocess_ctrl := -E
cc_compile_ctrl    := -S
cc_assemble_ctrl   := -c

ar_exe             := $(AR)
ar_flags           := cr

ld_exe             := $(LD)
ld_flags_front     := -m i386pep --wrap _Znwm --wrap _Znam --wrap _ZdlPv --wrap _ZdaPv --wrap _ZnwmRKSt9nothrow_t --wrap _ZnamRKSt9nothrow_t --wrap _ZdlPvRKSt9nothrow_t --wrap _ZdaPvRKnothrow_t -Bdynamic --dll-search-prefix=cyg --tsaware /usr/lib/gcc/x86_64-pc-cygwin/4.8.1/../../../../lib/crt0.o /usr/lib/gcc/x86_64-pc-cygwin/4.8.1/crtbegin.o -L/usr/lib/gcc/x86_64-pc-cygwin/4.8.1 -L/usr/lib/gcc6_64-pc-cygwin/4.8.1/../../../../x86_64-pc-cygwin/lib/../lib -L/usr/lib/gcc/x86_64-pc-cygwin/4.8.1/../../../../lib -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-pc-cygwin/4.8.1/../../../../x86_64-pc-cygwiib -L/usr/lib/gcc/x86_64-pc-cygwin/4.8.1/../../..
ld_flags_back      := -lgcc_s -lgcc -lcygwin -ladvapi32 -lshell32 -luser32 -lkernel32 -lgcc_s -lgcc /usr/lib/gcc/x86_64-pc-cygwin/4.8.1/crtend.o

endif

ifeq ($(platform),WIN32)
# not tested yet.

exe_path           := C:\Programs\Haskell\2013.2.0.0\mingw\bin

cc_exe             := C:\Programs\Haskell\2013.2.0.0\mingw\bin\realgcc.exe
cc_flags           :=
cc_dependency_ctrl := -MM
cc_preprocess_ctrl := -E
cc_compile_ctrl    := -S
cc_assemble_ctrl   := -c

ar_exe             := C:\Programs\Haskell\2013.2.0.0\mingw\bin\ar.exe
ar_flags           := cr

ld_exe             := C:\Programs\Haskell\2013.2.0.0\mingw\bin\ld.exe
ld_flags_front     := -Bdynamic C:/Programs/Haskell/2013.2.0.0/mingw/bin/../lib/crt2.o C:/Programs/Haskell/2013.2.0.0/mingw/bin/../lib/gcc/mingw32/4.5.2/crtbegin.o -LC:/Programs/Haskell/2013.2.0.0/mingw/bin -LC:/Programs/Haskell/2013.2.0.0/mingw/bin/../lib -LC:/Programs/Haskell/2013.2.0.0/mingw/bin/../lib/gcc/mingw32/4.5.2 -LC:/Programs/Haskell/2013.2.0.0/mingw/bin/../libexec/gcc/mingw32/4.5.2 -Lc:/programs/haskell/2013.2.0.0/mingw/bin/../lib/gcc/mingw32/4.5.2 -Lc:/programs/haskell/2013.2.0.0/mingw/bin/../lib/gcc -Lc:/programs/haskell/2013.2.0.0/mingw/bin/../lib/gcc/mingw32/4.5.2/../../../../mingw32/lib -Lc:/programs/haskell/2013.2.0.0/mingw/bin/../lib/gcc/mingw32/4.5.2/../../..
ld_flags_end       := -lmingw32 -lgcc_eh -lgcc -lmoldname -lmingwex -lmsvcrt -luser32 -lkernel32 -ladvapi32 -lshell32 -lmingw32 -lgcc_eh -lgcc -lmoldname -lmingwex -lmsvcrt C:/Programs/Haskell/2013.2.0.0/mingw/bin/../lib/gcc/mingw32/4.5.2/crtend.o

endif
