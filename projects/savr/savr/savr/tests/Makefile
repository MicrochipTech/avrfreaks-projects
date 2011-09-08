SUBDIRS= w1_test clock_test eth lcd

.PHONY: all clean $(SUBDIRS)

all clean:
	for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir $@; \
	done

