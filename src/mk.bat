attrib -r ..\include\version.h
..\bin\reversion ..\include\version.h %1 %2 %3 %4

attrib -r linkmain.lnk
..\bin\relink linkmain.lnk %1 %2 %3 %4

..\bin\make
..\bin\namebin main.bin %1 %2 %3 %4
