tftp -i %1 get phonebook.dat
convert -P -f phonebook.dat phonebook.txt
attrib -r phonebook.dat
del phonebook.dat
phonebook.txt
