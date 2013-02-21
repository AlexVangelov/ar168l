REM call getopt.bat %1
convert -p -f phonebook.txt phonebook.dat
tftp -i %1 put phonebook.dat
del phonebook.dat
del phonebook.txt

