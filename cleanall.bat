del *.scc
del *.bak
del *.hex
del *.bin
del *.zip
del *.tmp
del *.rar
del *.txt
attrib -r *.dat
del *.dat
rd release /s /q

cd bin
del *.scc
del *.txt
attrib -r *.dat
del *.dat
cd ..

cd doc
del *.scc
del *.bak
cd ..

cd include
del *.scc
del *.bak
cd ..

cd lib
del *.scc
cd ..

cd mcs51
call clean.bat
cd ..

cd src
call clean.bat
cd ..

cd tool
del *.bak
del *.ncb
del *.scc
del *.plg
del *.aps
del *.opt
rd debug /s /q
rd release /s /q

	cd convert
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd common
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd d2text
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd font
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd hex2bin
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd manager
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	cd res
		del *.scc
	cd ..
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd mergebin
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd namebin
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
	cd reversion
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..

	cd relink
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..

	cd adsptool
	del *.bak
	del *.ncb
	del *.scc
	del *.plg
	del *.aps
	del *.user
	rd debug /s /q
	rd release /s /q
	cd ..
	
cd ..

if not exist account goto end

cd account
del *.scc
cd ..

cd lib
del *.scc
del *.o
del *.rel
rd chip_plus /s /q
rd dual_mic /s /q
rd ivr /s /q
rd key_5x5 /s /q
rd key_8x5 /s /q
rd ksz8842 /s /q
rd sst /s /q
rd uart /s /q
cd ..

cd src
	cd res
		cd bt2008
			cd cn
			del *.scc
			cd ..
			
			cd us
			del *.scc
			cd ..
		cd ..
	cd..
cd ..
	
cd dsp
del *.scc
del *.bak
del *.map
del *.exe
del *.sym
del *.obj
del *.cde
del *.int
	cd common
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd data
		del *.scc
	cd ..
	cd ..
	
	cd dejitter
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd ..

	cd g721
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd ..

	cd g722
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd ..

	cd gsm
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd ..

	cd ilbc
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd data
		del *.scc
	cd..
	cd ..

	cd lpc
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd data
		del *.scc
	cd..
	cd ..

	cd g729
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd data
		del *.scc
	cd..
	cd..
	
	cd speex
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd data
		del *.scc
	cd..
	cd..
	
	cd myplc
	del *.scc
	del *.bak
	del *.obj
	del *.cde
	del *.int
	cd ..

	cd doc
	del *.scc
	del *.bak
	cd ..
	
cd ..
:end
