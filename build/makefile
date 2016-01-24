UNAME := $(shell uname)
ifeq ($(UNAME),$(filter $(UNAME),Linux Darwin FreeBSD GNU/kFreeBSD))
ifeq ($(UNAME),$(filter $(UNAME),Darwin))
GENIE?=./genie_mac
else
ifeq ($(UNAME),$(filter $(UNAME),FreeBSD GNU/kFreeBSD))
GENIE?=./genie_linux
else
GENIE?=./genie_linux
endif
endif
else
GENIE?=./genie
endif

all:
	$(GENIE) gmake
	$(MAKE) -R -C ./gmake config=release32
	$(MAKE) -R -C ./gmake config=release64
	$(MAKE) -R -C ./gmake config=debug32
	$(MAKE) -R -C ./gmake config=debug64

release32:
	$(GENIE) gmake
	$(MAKE) -R -C ./gmake config=release32

release64:
	$(GENIE) gmake
	$(MAKE) -R -C ./gmake config=release64

debug32:
	$(GENIE) gmake
	$(MAKE) -R -C ./gmake config=debug32

debug64:
	$(GENIE) gmake
	$(MAKE) -R -C ./gmake config=debug64