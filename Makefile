# Makefile
TARGET= $(eval T += $1)$(eval O += $(patsubst %,%.o,$2))$(eval H.$1 := $(patsubst %,%.h,$3))$(eval O.$1 := $2)
#CONFIG_UUID=f4d4bafd-661f-4823-8e5f-461b118ab1cb

# ---

$(call TARGET,tested-device, \
	start switchroot, \
	src/tested-device)

# ---

ifdef CONFIG_UUID
 .CONFIG_UUID:= echo $(subst -,,$(CONFIG_UUID)) | cut -c
 DCONFIG_UUID:= -DCONFIG_UUID_LOW=$(shell $(.CONFIG_UUID) 1-16) -DCONFIG_UUID_HIGH=$(shell $(.CONFIG_UUID) 17-32)
else
 DCONFIG_UUID:= -DCONFIG_UUID_LOW=0000000000000000 -DCONFIG_UUID_HIGH=0000000000000000
endif

CPPFLAGS += \
	$(DCONFIG_UUID)
CFLAGS += -Wall -Wextra -Werror
LDFLAGS += -static

# ---

%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: all force clean
all: $T
force: clean | all
clean:
	$(RM) $T $O

define .TARGET
$1: $(patsubst %,%.o,$(O.$1))
	$(CC) $(LDFLAGS) -o $$@ $$^ $(LIBS)
endef

$(foreach .T,$T,$(eval $(call .TARGET,$(.T))) \
	$(foreach .O,$(O.$(.T)),$(eval $(.O).o: src/$(.O).c $(H.$(.T)))))

