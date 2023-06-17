# The MIT License
#
# Copyright (c) 2022 wstux
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

export NPROC	?= $(shell (nproc))
export NJOB	?= $(shell expr '(' $(NPROC) + 1 ')')

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
#	@echo "$(PATH)"

endef

$(eval $(call common_rule,debug))
$(eval $(call common_rule,release))

