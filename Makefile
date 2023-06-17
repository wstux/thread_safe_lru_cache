COMMON_MK	:= cmake/common.mk

.PHONY: default
default: release/all

ifeq ($(shell test -e $(COMMON_MK) && echo -n yes),yes)
include $(COMMON_MK)
else
$(info ERROR: $(COMMON_MK) not found)
endif

.PHONY: all
all: release/all

$(eval $(call rule,debug))
$(eval $(call rule,release))

