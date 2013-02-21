attrib -r pm.dat
del pm.dat
tftp -i %1 get pm.dat
