all:	clean reversion relink namebin hex2bin

hex2bin: hex2bin/hex2bin.o hex2bin/HexFile.o
	g++ hex2bin/hex2bin.o hex2bin/HexFile.o -o ../bin/hex2bin

namebin: namebin/namebin.o common/common.o common/cmd_default.o
	g++ namebin/namebin.o common/common.o common/cmd_default.o -o ../bin/namebin

relink:  relink/relink.o common/common.o common/cmd_default.o
	g++ relink/relink.o common/cmd_default.o -o ../bin/relink

reversion: reversion/reversion.o common/common.o common/cmd_default.o
	g++ reversion/reversion.o common/common.o common/cmd_default.o -o ../bin/reversion

common:	common/common.o cmd_default
	g++ -c common/common.o

cmd_default: common/cmd_default.o
	g++ -c common/cmd_default.o
    
clean:
	rm -rf namebin/namebin.o relink/*.o reversion/*.o common/*.o hex2bin/*.o ../bin/reversion ../bin/relink ../bin/namebin ../bin/hex2bin
