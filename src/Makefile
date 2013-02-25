ifdef SystemRoot
PATH_SDCC=C:\SDCC

RM = del
AZ80 = $(PATH_SDCC)\BIN\sdasz80
CZ80 = $(PATH_SDCC)\BIN\sdcc
LZ80 = $(PATH_SDCC)\BIN\sdld
HEX2BIN = $(PATH_SDCC)\BIN\hex2bin

CZ80_OPT=-mz80 -c --std-c99 --codeseg CODE
AZ80_OPT=-plosgff

VERSION_FILE = $(PATH_SDCC)\include\version.h
LINKMAIN = linkmain
else
RM = rm
AZ80 = sdasz80
CZ80 = sdcc
LZ80 = sdld
HEX2BIN = ../bin/hex2bin
INC = -I../include

CZ80_OPT=-mz80 $(INC) -c --std-c99 --codeseg CODE
AZ80_OPT=-plosgff

VERSION_FILE = ../include/version.h
LINKMAIN = linkmain_unix
endif
%.rel:	%.c
	$(CZ80) $*.c $(CZ80_OPT)

SRAM_OBJS = main.rel ce3.rel datatype.rel isr_gpio.rel ksz8842.rel lcd.rel math.rel ne2000.rel
FONT_OBJS = font.rel
FLASH0_OBJS = buf.rel data.rel heap.rel line.rel misc.rel options.rel rand.rel sipc.rel syslog.rel udpdebug.rel
FLASH1_OBJS = adc_key.rel ar168r.rel dtmf.rel function.rel gpio_key.rel hook.rel ivr.rel key.rel led.rel serial.rel time.rel
FLASH2_OBJS = dhcp.rel dns.rel net.rel ppp.rel pppoe.rel sntp.rel stun.rel tftp.rel udprun.rel
FLASH3_OBJS = http.rel httpc.rel ap_innom.rel
FLASH4_OBJS = call_log.rel menu.rel settings.rel ui_str.rel
FLASH5_OBJS = iax2.rel protocol.rel rtps.rel sips.rel
FLASH6_OBJS = auth.rel md5.rel rc4.rel sha1.rel hmac_sha1.rel pbkdf2_sha1.rel
FLASH7_OBJS = digitmap.rel rtpr.rel sipr.rel


all:	main.bin

main.rel: main.c $(VERSION_FILE)
	$(CZ80) main.c $(CZ80_OPT)0
	
ce3.rel: ce3.c $(VERSION_FILE)
	$(CZ80) ce3.c $(CZ80_OPT)0
	
datatype.rel: datatype.s
	$(AZ80) $(AZ80_OPT) datatype.rel $^
	
isr_gpio.rel: isr_gpio.c $(VERSION_FILE)
	$(CZ80) isr_gpio.c $(CZ80_OPT)0
	
ksz8842.rel: ksz8842.c $(VERSION_FILE)
	$(CZ80) ksz8842.c $(CZ80_OPT)0

lcd.rel: lcd.c $(VERSION_FILE)
	$(CZ80) lcd.c $(CZ80_OPT)0

math.rel: math.s
	$(AZ80) $(AZ80_OPT) math.rel $^

ne2000.rel: ne2000.c $(VERSION_FILE)
	$(CZ80) ne2000.c $(CZ80_OPT)0
	
font.rel: font.c $(VERSION_FILE)
	$(CZ80) font.c $(CZ80_OPT)F
	
heap.rel: heap.c $(VERSION_FILE)
	$(CZ80) heap.c $(CZ80_OPT)
	
sipc.rel: sipc.c $(VERSION_FILE)
	$(CZ80) sipc.c $(CZ80_OPT)

adc_key.rel: adc_key.c $(VERSION_FILE)
	$(CZ80) adc_key.c $(CZ80_OPT)1
	
ar168r.rel: ar168r.c $(VERSION_FILE)
	$(CZ80) ar168r.c $(CZ80_OPT)1
	
dtmf.rel: dtmf.c $(VERSION_FILE)
	$(CZ80) dtmf.c $(CZ80_OPT)1
	
function.rel: function.c $(VERSION_FILE)
	$(CZ80) function.c $(CZ80_OPT)1
	
gpio_key.rel: gpio_key.c $(VERSION_FILE)
	$(CZ80) gpio_key.c $(CZ80_OPT)1
	
hook.rel: hook.c $(VERSION_FILE)
	$(CZ80) hook.c $(CZ80_OPT)1
	
ivr.rel: ivr.c $(VERSION_FILE)
	$(CZ80) ivr.c $(CZ80_OPT)1
	
key.rel: key.c $(VERSION_FILE)
	$(CZ80) key.c $(CZ80_OPT)1
	
led.rel: led.c $(VERSION_FILE)
	$(CZ80) led.c $(CZ80_OPT)1 --disable-warning 110
	
serial.rel: serial.c $(VERSION_FILE)
	$(CZ80) serial.c $(CZ80_OPT)1
	
time.rel: time.c $(VERSION_FILE)
	$(CZ80) time.c $(CZ80_OPT)1
	
dhcp.rel: dhcp.c $(VERSION_FILE)
	$(CZ80) dhcp.c $(CZ80_OPT)2
	
dns.rel: dns.c
	$(CZ80) $^ $(CZ80_OPT)2

net.rel: net.c $(VERSION_FILE)
	$(CZ80) net.c $(CZ80_OPT)2
	
ppp.rel: ppp.c
	$(CZ80) $^ $(CZ80_OPT)2
		
pppoe.rel: pppoe.c $(VERSION_FILE)
	$(CZ80) pppoe.c $(CZ80_OPT)2
		
sntp.rel: sntp.c
	$(CZ80) $^ $(CZ80_OPT)2

stun.rel: stun.c $(VERSION_FILE)
	$(CZ80) stun.c $(CZ80_OPT)2
	
tftp.rel: tftp.c $(VERSION_FILE)
	$(CZ80) tftp.c $(CZ80_OPT)2
	
udprun.rel: udprun.c $(VERSION_FILE)
	$(CZ80) udprun.c $(CZ80_OPT)2
		
http.rel: http.c $(VERSION_FILE)
	$(CZ80) http.c $(CZ80_OPT)3

httpc.rel: httpc.c
	$(CZ80) $^ $(CZ80_OPT)3
	
ap_innom.rel: ap_innom.c $(VERSION_FILE)
	$(CZ80) ap_innom.c $(CZ80_OPT)3
	
call_log.rel: call_log.c $(VERSION_FILE)
	$(CZ80) call_log.c $(CZ80_OPT)4

menu.rel: menu.c $(VERSION_FILE)
	$(CZ80) menu.c $(CZ80_OPT)4
	
settings.rel: settings.c $(VERSION_FILE)
	$(CZ80) settings.c $(CZ80_OPT)4
	
ui_str.rel: ui_str.c $(VERSION_FILE)
	$(CZ80) ui_str.c $(CZ80_OPT)4
	
iax2.rel: iax2.c $(VERSION_FILE)
	$(CZ80) iax2.c $(CZ80_OPT)5

protocol.rel: protocol.c
	$(CZ80) $^ $(CZ80_OPT)5

rtps.rel: rtps.c $(VERSION_FILE)
	$(CZ80) rtps.c $(CZ80_OPT)5
	
sips.rel: sips.c $(VERSION_FILE)
	$(CZ80) sips.c $(CZ80_OPT)5
	
auth.rel: auth.c
	$(CZ80) $^ $(CZ80_OPT)6

md5.rel: md5.c
	$(CZ80) $^ $(CZ80_OPT)6

rc4.rel: rc4.c $(VERSION_FILE)
	$(CZ80) rc4.c $(CZ80_OPT)6

sha1.rel: sha1.c $(VERSION_FILE)
	$(CZ80) sha1.c $(CZ80_OPT)6

hmac_sha1.rel: hmac_sha1.c $(VERSION_FILE)
	$(CZ80) hmac_sha1.c $(CZ80_OPT)6

pbkdf2_sha1.rel: pbkdf2_sha1.c $(VERSION_FILE)
	$(CZ80) pbkdf2_sha1.c $(CZ80_OPT)6

digitmap.rel: digitmap.c
	$(CZ80) $^ $(CZ80_OPT)7
	
rtpr.rel: rtpr.c $(VERSION_FILE)
	$(CZ80) rtpr.c $(CZ80_OPT)7

sipr.rel: sipr.c $(VERSION_FILE)
	$(CZ80) sipr.c $(CZ80_OPT)7

main.ihx: $(SRAM_OBJS) $(FONT_OBJS) $(FLASH0_OBJS) $(FLASH1_OBJS) $(FLASH2_OBJS) $(FLASH3_OBJS) $(FLASH4_OBJS) $(FLASH5_OBJS) $(FLASH6_OBJS) $(FLASH7_OBJS)
	$(LZ80) -n -f $(LINKMAIN)

main.bin: main.ihx
	$(HEX2BIN) main.bin -f0 -a0 -x6 -b32768 -e57344 main.ihx
	
clean:
	$(RM) -f *.lst *.rel *.ihx *.sym *.asm *.map *.bin *.noi linkmain_unix.lnk
