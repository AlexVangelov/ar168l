attrib -r dm.dat
del dm.dat
tftp -i %1 get dm.dat
