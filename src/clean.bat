del *.ihx
del *.o
del *.noi
del *.rel
del *.hex
del *.sym
del *.map
del *.asm
del *.lst
del *.bak
del *.bin
del *.rar
del *.scc
del *.txt
del *.zip
attrib -r *.dat
del *.dat
del safemode.h
rd debug /s /q
rd release /s /q

cd res
del *.scc
del *.bak
	cd cn
	del *.scc
	del *.bak
	cd ..
	cd es
	del *.scc
	del *.bak
	cd ..
	cd fr
	del *.scc
	del *.bak
	cd ..
	cd it
	del *.scc
	del *.bak
	cd ..
	cd ro
	del *.scc
	del *.bak
	cd ..
	cd ru
	del *.scc
	del *.bak
	cd ..
	cd tr
	del *.scc
	del *.bak
	cd ..
	cd us
	del *.scc
	del *.bak
	cd ..
cd ..

cd settings
del *.scc
del *.bak
cd..
