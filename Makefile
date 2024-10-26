export NPROC ?= $(shell (nproc))
export NJOB  ?= $(shell expr '(' $(NPROC) + 1 ')')

.PHONY: default
default: release/all

# Common rule declaration
define common_rule

build_$(1)/Makefile:
	@mkdir -p $$(@D) && cd $$(@D) && cmake -DCMAKE_BUILD_TYPE="$(1)" ../

$(1)/%: build_$(1)/Makefile
	@make -j $(NJOB) --output-sync=target --no-print-directory -C build_$(1) $$*

$(1): $(1)/all
#	@make -j $(NJOB) --output-sync=target --no-print-directory -C build_$(1) $$*

.PHONY: $(1)/clean
$(1)/clean:
	@rm -rf build_$(1)

.PHONY: $(1)/test
$(1)/test: $(1)/all
	@make -j $(NJOB) -C build_$(1) test

# Is not implemented now
#.PHONY: $(1)/coverage
#$(1)/coverage: $(1)/all
#	@make -j $(NJOB) -C build_$(1) test

endef

# Debug and release build types definition
$(eval $(call common_rule,debug))
$(eval $(call common_rule,release))
#$(eval $(call common_rule,coverage))

.PHONY: all
all: release/all

.PHONY: clean
clean: release/clean

#.PHONY: coverage
#coverage: coverage/coverage

.PHONY: test
test: release/test

