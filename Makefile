SDCC_SVN_IMMEDIATE_DIRS = src sdas sdas/asgb sdas/linksrc sdas/asranlib sdas/asxxsrc doc device device/include device/non-free device/non-free/include support support/librarian support/makebin support/regression support/regression/ports support/regression/ports/host support/valdiag support/scripts support/Util
SDCC_SVN_INFINITY_DIRS = src/z80 sdas/asz80 support/cpp
SDCC_REVISION = 6078
SDCC_BUILD_DIR = sdcc_build

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

sdcc: sdcc_checkout sdcc_build sdcc_install

sdcc_checkout:
	svn co -r $(SDCC_REVISION) https://sdcc.svn.sourceforge.net/svnroot/sdcc/trunk/sdcc $(SDCC_BUILD_DIR) --depth=immediates;
	cd $(SDCC_BUILD_DIR); svn update -r $(SDCC_REVISION) --set-depth=immediates $(SDCC_SVN_IMMEDIATE_DIRS);
	cd $(SDCC_BUILD_DIR); svn update -r $(SDCC_REVISION) --set-depth=infinity $(SDCC_SVN_INFINITY_DIRS);

sdcc_build:
	cd $(SDCC_BUILD_DIR); ./configure --disable-mcs51-port --disable-gbz80-port --disable-ds390-port --disable-ds400-port --disable-pic14-port --disable-pic16-port --disable-hc08-port --disable-ucsim --disable-device-lib --disable-packihx --disable-sdcdb;
	$(MAKE) -C $(SDCC_BUILD_DIR);

sdcc_install:
	ln -rvs $(SDCC_BUILD_DIR)/bin/sdcc bin/;
	ln -rvs $(SDCC_BUILD_DIR)/bin/sdasz80 bin/;
	ln -rvs $(SDCC_BUILD_DIR)/bin/sdld bin/;
	ln -rvs $(SDCC_BUILD_DIR)/bin/sdcpp bin/;

sdcc_uninstall:
	rm -Rf bin/sdcc bin/sdasz80 bin/sdld bin/sdcpp;

sdcc_clean:
	$(MAKE) -C $(SDCC_BUILD_DIR) clean;

.PHONY: tool sdcc sdcc_checkout sdcc_build sdcc_install sdcc_uninstall sdcc_clean

clean:
	$(MAKE) -C tool clean
	$(MAKE) -C tool uninstall
	$(MAKE) -C src clean
	
