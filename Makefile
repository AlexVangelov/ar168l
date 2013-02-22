
all: tool install demo

tool:
	$(MAKE) -C tool all

install:
	$(MAKE) -C tool install

demo: 	
	@echo
	@echo "***************************************";
	@echo "***      Example build AR168M";
	@echo "*** To build another configuration:";
	@echo "*** look at the full command below ;)";
	@echo "***************************************";
	cd src; chmod +x mk; ./mk ar168m sip us

.PHONY: tool

clean:
	$(MAKE) -C tool clean
	$(MAKE) -C src clean
