export AM_HOME := $(PWD)/../../abstract-machine

ifeq ($(ARCH),)
export ARCH    := native
endif
.PHONY: all run clean latest $(ALL)

RESULT = .result
$(shell > $(RESULT))

COLOR_RED   = \033[1;31m
COLOR_GREEN = \033[1;32m
COLOR_NONE  = \033[0m

ALL = $(basename $(notdir $(shell find tests/. -name "*.c")))

all: $(addprefix Makefile., $(ALL))
	@echo "" $(ALL)

$(ALL): %: Makefile.%

### for qemu target, halt(1) will not pass through qemu return code
### get halt code by grep its serial output. (Line 27)
Makefile.%: tests/%.c latest
	@/bin/echo -e "NAME = $*\nSRCS = $<\nLIBS += klib\ninclude $(AM_HOME)/Makefile" > $@
	@if make -s -f $@ ARCH=$(ARCH) $(MAKECMDGOALS) > .tmp ; then \
		if [ "$(ARCH)" != "native" ] && [ $$(grep "CPU" .tmp | grep -Eo "\([0-9]{2}\)") != "(00)" ]; then \
			printf "[%14s] $(COLOR_RED)FAIL!$(COLOR_NONE)\n" $* >> $(RESULT); \
		else \
			printf "[%14s] $(COLOR_GREEN)PASS!$(COLOR_NONE)\n" $* >> $(RESULT); \
		fi \
	else \
		printf "[%14s] $(COLOR_RED)FAIL!$(COLOR_NONE)\n" $* >> $(RESULT); \
	fi
	-@rm -f Makefile.$*
	-@rm -f .tmp

run: all
	@cat $(RESULT)
	@rm $(RESULT)

clean:
	rm -rf Makefile.* build/

latest:
