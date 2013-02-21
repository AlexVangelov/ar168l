PATH_SDCC = C:\SDCC

RM = del
C51 = $(PATH_SDCC)\BIN\sdcc
A51 = $(PATH_SDCC)\BIN\sdas8051
HEX2BIN = $(PATH_SDCC)\BIN\hex2bin

#CFLAGS = -V
AFLAGS = -plosgff
#MFLAGS = -mmcs51 --model-large
MFLAGS = -mmcs51 --model-small
LFLAGS = --nostdlib --xram-loc 0x0000 --code-loc 0x0000

%.rel: %.c
	$(C51) -c $(CFLAGS) $(MFLAGS) $<

%.rel: %.a51
	$(A51) $(AFLAGS) $<

all: main.bin

OBJS = main.rel hook.rel key.rel lcd.rel led.rel string.rel uart.rel ui.rel crtstart.rel

main.ihx: $(OBJS)
	$(C51) $(MFLAGS) $(LFLAGS) $(OBJS)

main.bin: main.ihx
	$(HEX2BIN) main.bin -f255 -a0 main.ihx
	
clean:
	$(RM) *.bak *.lst *.rel *.ihx *.sym *.asm *.map *.lnk *.~*

