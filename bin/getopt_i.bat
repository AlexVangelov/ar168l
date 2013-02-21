tftp -i %1 get options.dat
convert -C --innomedia -f options.dat options.txt
attrib -r options.dat
del options.dat
options.txt
