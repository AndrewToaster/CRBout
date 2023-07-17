STD += -std=gnu11

define mkdir
	mkdir -p $(1)
endef

define rm
	rm -rf $(1)
endef