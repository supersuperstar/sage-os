SHELL := /bin/bash
export AM_HOME := $(PWD)/../abstract-machine

ifeq ($(ARCH),)
export ARCH    := x86_64-qemu
endif

ifeq ($(smp),)
export smp	   := 2
endif

ifeq ($(LOG_MASK),)
export LOG_MASK := 14
endif

CFLAGS_EXTRA += "-DLOG_MASK=$(LOG_MASK)"

.PHONY: all run debug clean latest $(ALL)

RESULT = .result
$(shell > $(RESULT))

COLOR_RED   = \033[1;31m
COLOR_GREEN = \033[1;32m
COLOR_NONE  = \033[0m

ALL = $(basename $(notdir $(shell find units/. -name "test_*.c")))

all: $(addprefix Makefile., $(ALL))
	@printf "\nTargets:\n$(ALL)\n"

$(ALL): %: Makefile.%

### for qemu target, halt(1) will not pass through qemu return code
### get halt code by grep its serial output. (Line 27)
### will skip running _*.c files
Makefile.%: units/%.c latest
	@if [ "$(NOKERNEL)" = "true" ] ; then \
		/bin/echo -e "NAME = $*\nSRCS = $<\nLIBS += klib\nCFLAGS += -DTEST $(CFLAGS_EXTRA)\ninclude $(AM_HOME)/Makefile" > $@; \
	else \
		/bin/echo -e "NAME = $*\nSRCS = $< $(shell find -L ../kernel/src/ -name "*.c")\nINC_PATH = include ../kernel/include/ ../kernel/framework/\nCFLAGS += -DTEST $(CFLAGS_EXTRA)\ninclude $(AM_HOME)/Makefile" > $@; \
	fi
	@mkdir -p out
	@printf "\n========== Unit test start: %s\n" $*
	@if make -s -f $@ ARCH=$(ARCH) run > out/$*.out ; then \
		cat out/$*.out; \
		if [ "$(ARCH)" != "native" ] && [ $$(grep "CPU" out/$*.out | grep -Eo "\([0-9]{2}\)") != "(00)" ]; then \
			printf "========== $(COLOR_RED)FAIL!$(COLOR_NONE)\n"; \
			printf "[%14s] $(COLOR_RED)FAIL!$(COLOR_NONE)\n" $* >> $(RESULT); \
		else \
			printf "========== $(COLOR_GREEN)PASS!$(COLOR_NONE)\n"; \
			printf "[%14s] $(COLOR_GREEN)PASS!$(COLOR_NONE)\n" $* >> $(RESULT); \
		fi \
	else \
		printf "[%14s] $(COLOR_RED)FAIL!$(COLOR_NONE)\n" $* >> $(RESULT); \
	fi

### build single test unit
### must 
build: latest
	@if [ -z "$(target)" ]; then \
		printf "Must specify target!\n"; \
		printf "Usage: make build target=xxx ARCH=xxx\n"; \
		exit 1; \
	else \
		if [ "$(NOKERNEL)" = "true" ] ; then \
			/bin/echo -e "NAME = $(target)\nSRCS = units/$(target).c\nLIBS += klib\nCFLAGS += -DTEST $(CFLAGS_EXTRA)\ninclude $(AM_HOME)/Makefile" > Makefile.$(target); \
		else \
			/bin/echo -e "NAME = $(target)\nSRCS = units/$(target).c $(shell find -L ../kernel/src/ -name "*.c")\nINC_PATH = include ../kernel/include/ ../kernel/framework/\nCFLAGS += -DTEST $(CFLAGS_EXTRA)\ninclude $(AM_HOME)/Makefile" > Makefile.$(target); \
		fi \
	fi
	@make -s -f Makefile.$(target) ARCH=$(ARCH)

### run single test unit
run: clean build latest
	@if [ "$(ARCH)" = "x86_64-qemu" ]; then \
		qemu-system-x86_64 \
			  -serial mon:stdio \
			  -machine accel=tcg \
              -smp $(smp) \
              -drive format=raw,file="build/$(target)-x86_64-qemu"; \
	else \
		./build/$(target)-native; \
	fi

### debug single test unit
debug: clean build latest
	@if [ "$(ARCH)" = "x86_64-qemu" ]; then \
		qemu-system-x86_64 \
			  -serial mon:stdio \
              -machine accel=tcg \
              -smp $(smp) \
              -drive format=raw,file=build/$(target)-x86_64-qemu \
			  -s -S; \
	else \
		gdb -ex "handle SIGUSR1 SIGUSR2 SIGSEGV noprint nostop" build/$(target)-native; \
	fi

test: clean all
	@printf "\nTest Result:\n"
	@cat $(RESULT)
	@if [[ `cat $(RESULT)` == *"FAIL!"* ]] ; then exit 1; fi
	@rm $(RESULT)
	-@rm -f Makefile.$*

clean:
	rm -rf .result Makefile.* build/ out/

latest:
