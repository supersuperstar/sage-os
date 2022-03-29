SHELL := /bin/bash
export AM_HOME := $(PWD)/../abstract-machine

ifeq ($(ARCH),)
export ARCH    := native
endif

ifeq ($(smp),)
export smp	   := 1
endif

.PHONY: all run debug clean latest $(ALL)

RESULT = .result
$(shell > $(RESULT))

COLOR_RED   = \033[1;31m
COLOR_GREEN = \033[1;32m
COLOR_NONE  = \033[0m

ALL = $(basename $(notdir $(shell find units/. -name "*.c")))

all: $(addprefix Makefile., $(ALL))
	@printf "\nTargets:\n$(ALL)\n"

$(ALL): %: Makefile.%

### for qemu target, halt(1) will not pass through qemu return code
### get halt code by grep its serial output. (Line 27)
Makefile.%: units/%.c latest
	@if [ "$(NOKERNEL)" = "true" ] ; then \
		/bin/echo -e "NAME = $*\nSRCS = $<\nLIBS += klib\nCFLAGS+=-DTEST\ninclude $(AM_HOME)/Makefile" > $@; \
	else \
		/bin/echo -e "NAME = $*\nSRCS = $< $(shell find -L ../kernel/src/ -name "*.c")\nINC_PATH = ../kernel/include/ ../kernel/framework/\nCFLAGS+=-DTEST\ninclude $(AM_HOME)/Makefile" > $@; \
	fi
	@mkdir -p out
	@printf "\n========== Unit test start: %s\n" $*
	@if make -s -f $@ ARCH=$(ARCH) $(MAKECMDGOALS) > out/$*.out ; then \
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
			/bin/echo -e "NAME = $(target)\nSRCS = units/$(target).c\nLIBS += klib\nCFLAGS+=-DTEST\ninclude $(AM_HOME)/Makefile" > Makefile.$(target); \
		else \
			/bin/echo -e "NAME = $(target)\nSRCS = units/$(target).c $(shell find -L ../kernel/src/ -name "*.c")\nINC_PATH = ../kernel/include/ ../kernel/framework/\nCFLAGS+=-DTEST\ninclude $(AM_HOME)/Makefile" > Makefile.$(target); \
		fi \
	fi
	@make -s -f Makefile.$(target) ARCH=$(ARCH)

### debug single test unit
debug: build latest
	@if [ "$(ARCH)" = "x86_64-qemu" ]; then \
		qemu-system-x86_64 \
			  -serial mon:stdio \
              -machine accel=tcg \
              -smp "$(smp)" \
              -drive format=raw,file=build/$(target)-x86_64-qemu \
			  -s -S; \
	else \
		gdb -ex "handle SIGUSR1 SIGUSR2 SIGSEGV noprint nostop" build/$(target)-native; \
	fi

run: clean all
	@printf "\nTest Result:\n"
	@cat $(RESULT)
	@if [[ `cat $(RESULT)` == *"FAIL!"* ]] ; then exit 1; fi
	@rm $(RESULT)
	-@rm -f Makefile.$*

clean:
	rm -rf .result Makefile.* build/ out/

latest:
