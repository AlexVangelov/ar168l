DIRS = reversion relink hex2bin namebin font convert

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
		ln -rvs $$dir/$$dir ../bin/; \
	done

uninstall:
	@for dir in $(DIRS); do \
                rm -Rf ../bin/$$dir; \
        done
