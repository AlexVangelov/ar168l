DIRS = reversion relink hex2bin namebin

all clean:
	@for dir in $(DIRS); do \
		if $(MAKE) -C $$dir $@; then \
			true; \
		else \
			exit 1; \
		fi; \
	done

install:
	@for dir in $(DIRS); do \
		echo "*** Copy $$dir to bin"; \
		cp -RLf $$dir/$$dir ../bin/; \
	done
