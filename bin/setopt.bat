REM call getopt.bat %1
convert -c -f options.txt settings.dat
tftp -i %1 put settings.dat
del settings.dat
del options.txt

