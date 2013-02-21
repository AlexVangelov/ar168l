0.61 & 0.62 Revision list (Apr 1, 2013):
1. Added OEM_LCD4 compile option for AR168M to use dot matrix LCD.
2. Added all keypad related source files into software API.


0.59 & 0.60 Revision list (Dec 15, 2012):
1. Added OEM_SIZE compile option for namebin.exe to report total code size.
2. Added OEM_ZIP compile option for namebin.exe to zip generated binary file.
3. Changed Z80 speed at 24Mhz with all AR1688 devices.
4. Added OEM_BROADCAST compile option for broadcast UDP package handling.
5. Added 7th TEXT command in our VoIP module high level UI protocols. 
6. Removed unmature hardware type VER_AR168L/VER_AR168LS and related SLIC code.
7. Removed unsteady hardware type VER_AR168O/VER_AR168KD and related DM9003 code.


0.57 & 0.58 Revision list (Aug 8, 2012):
1. Added OEM_ROIP compile option for AR168M users to develop Radio over IP without UART functions working.
2. Added OEM_IVRG729 compile option for demo source code of AR1688 G.729 IVR function.
3. Removed requirement of full 64k bytes .dat file when upgrade font and IVR data.
4. Added SyslogAllUdpData() function for easy SIP message debug display.
5. Added hardware type AR168MT for VoIP module with KSZ8842 ethernet chip and MT28F016S5 program flash.
6. Added UdpDebugTcpHead() fuction for TCP debug dispaly.
7. Added hardware type AR168R for Radio over IP module.


0.55 & 0.56 Revision list (Jan 28, 2012):
1. Fixed UNICODE message display bug for tools in C:\sdcc\bin.
2. Improved hex2bin.exe debug information output.
3. Added OEM_UART support for AR168G/GP1266/GP2266 IP phones for users to use those IP phones to test AR168M VoIP module functions.
4. Alex added sending text messages of type SIP_REQ_MESSAGE.
5. Added STATxxx status report command in VoIP module high level UI protocols.


0.53 & 0.54 Revision list (Aug 23, 2011):
1. Removed duplicated "Via" head check in SIP protocol, because the new ET China system will send duplicated "Via" head.
2. Rollback RTL8019AS over-optimization for better hardware compatibility.
3. Added "volatile USHORT Sys_sMinuteDebug" debug var and report non zero value in whole minute timer UI_MinuteTimer().
4. Added hardware type AR168MK for VoIP module with KSZ8842.
5. Removed "content-length" check in SIP protocol for an unknown system.


0.51 & 0.52 Revision list (Mar 25, 2011):
1. Removed DemoTcp.c from API, now we believe it is a bad idea to develop customer TCP application on AR1688.
2. Added ivr.c in API, for users to customize IVR support.
3. Reduced DTMF output in half.
4. Changed Manager.exe icon, pasted Palmtool.exe icon onto original VC6 MFC app icon.


0.49 & 0.50 Revision list (Feb 5, 2011):
1. SDCC compiler for Z80 and 8051 upgrade to 3.0.1, be aware of lots of source code changes because of the upgrade
2. Removed SHA1 and RC4 code from common software compilation to reduce code size and data usage
3. Added strcat_char() function to replace strcat() calls with single char
4. Improved syslog performance
5. Fixed ring bug in AR168M UI
6. Added --std-c99 compiler option to use C99 bool, BOOLEAN define changed from "unsigned char" to "_Bool"
7. Make Manager.exe to report AR1688 device even when debug is disabled on the device
8. Added DNS SRV support
9. Fred Villella added Australian daylight saving time


0.47 & 0.48 Revision list (Oct 22, 2010):
1. Fixed a code size check bug in hex2bin.exe
2. Change SIP SessionExpires and SessionTimer var from 16 bit to 32 bit
3. Fixed a memory leak bug in Manager.exe
4. Tools in SDCC\bin compiler upgraded from VC6 to VC9(VC2008)
5. Lay3Qos option field value put to RTP IP header directly without any mask


0.45 & 0.46 Revision list (May 7, 2010):
1. Handset down in safe mode will reboot and leave safe mode
2. Fixed a error key detection for YWH201
3. Fixed a missed call display bug for 2 line display phones
4. Fixed a demo UI bug in AR168M module
5. Change SIP User-Agent head to the same as LCD display and Menu
6. Change software API license terms to BSD license
7. Pedram separated normal text display TTL(3 seconds) and special text display TTL(20 seconds)
8. Palmmicro address in web settings change from www.palmmicro.com.cn to www.palmmicro.com
9. EnableLCD() source code move to lcd.c, available in API now
10. OptionsGetBuf() and OptionsGetByte() source code move to main.c, available in API now


0.43 & 0.44 Revision list (Oct 1, 2009):
1. SDCC compiler upgrade to 2.9.0
2. Fixed a UDP zero length data bug
3. Added "Dial Prefix" to call out numbers in all other situations besides 'call' key pressed
4. Seperated serial send and receive buffer size
5. Limited ethernet continue receive max 5 packets to reduce voice out packet jitter
6. Added DTMF detect in incoming voice stream
7. Vuong To¨¤n Dung added Vietnam support
8. Updated Manager.exe debug window title with IP address when "Debug IP Only"
9. Added "Ring Volume" option to seperate ring volume and speaker phone volume settings
10. Added UART IRQ mode select between "half full" and "anything received", removed none-IRQ mode UART support, UART baudrate changed back to 19200bps on both AR168M and MCS51


0.41 & 0.42 Revision list (Dec 16, 2008):
1. Added Micrel KSZ8842 (2-port switch with 8 bit CPU bus) support
2. Added prefix to number entered through call log and phonebook
3. Bug fixed: hooking when phone tries to connect DHCP server at startup makes the phone COLD BOOT
4. Bug fixed: when entering call log by pressing 'Call' key, and then save a call log entry to phonebook will corrupt the settings
5. Bug fixed: when the phone first boot up, the DSP is not working properly, so there is no dial tone or ringing tone etc
6. Added OEM_VLAN compile option to make special VLAN testing software
7. Added OEM_DEBUG compile option to make special debug software
8. Added VLAN support for DM9003 and KSZ8842
9. Added extensive assembly optimization for RTL8019AS/DM9003/KSZ8842 network performance
10. Removed speech codec module related files, future speech codec module application will use IAX2 or RTP protocol for compressed voice data instead of none standard format
11. Bug fixed: when pressing dtmf key quickly, one side will stop hearing the incoming voice
12. Let timestamp field in the voice to reflect the actual time when the packet is being sent
13. Fixed a IAX2 call hold bug
14. Matthias Siegele fixed a DTMF receive bug using SIP INFO


0.39 & 0.40 Revision list (Nov 24, 2008):
* 1-2 changes by Ferhat Olgun
1. Fixed a bug related with call log delete all records
2. Adjusted Turkish DTMF tones
3. SDCC compiler upgrade to 2.8.3
4. Volume change is effective until the phone reboots
5. Fixed a SIP Call Park bug
6. Fixed a SIP Call Hold bug
7. Finalized 1-port FXS gateway software, SLIC control source code moved to software API
8. RTL8019AS and DM9003 network control source code moved to software API
9. Disabled hardware ADC and DAC dither
10. Added RC4 stream cipher function and pbkdf2_sha1, a password-based key generation function
11. Added negotiation of Annex.B of G729 (VAD) through SDP


0.37 & 0.38 Revision list (Sep 3, 2008):
1. Added auto-provision for phonebook
2. Play congestion tone instead of dial tone if the phone doesn't logon successfully
3. Update a new ringtone.dat which has much better quality
4. UDP debug message replaced by standard RFC 3164 syslog
5. Voice codec speex is ready to use
6. GPIO control source code now included in software API
7. Multiple updates in sdcc\bin\manager.exe


0.35 & 0.36 Revision list (Aug 5, 2008):
1. Fixed a SIP INFO bug
2. Added speed dial name and number display together for 4-line displays
3. Added AR168M UART loop back serial template when read data in interrupt
4. Reduced AR168M and MCS51 UI UART baudrate from 19200 to 9600 to avoid MCS51 data overflow
5. Added SIP session timer support.(RFC4028)
6. Finalized AR168L and AR168LS 1-port FXS gateway design
7. Added 16x16 Korean and Japanese font
8. Fixed an IAX2 0-length voice frame bug
9. PPPoE and PPP source code now included in software API
10. Fixed a SIP REFER bug.


0.33 & 0.34 Revision list (Jun 26, 2008):
1. Fixed an IAX2 "Send Text" command bug
2. Added load and store default settings function in main program menu
3. Moved GP1260(AR168G)/GP1266/GP2266 factory test to main program menu
4. Omitted receiving UDP&TCP check sum check to boost performance
5. Added Davicom DM9003 support
6. Fixed an ip fragment memory allocation bug
7. Improved G.726 32kpbs voice quality
8. Added call-waiting tone
9. Changed part of SetHighSpeed() to more detailed SetZ80Speed() function
10. Improved Speex performance


0.31 & 0.32 Revision list (May 16, 2008):
1. SDCC compiler upgrade to 2.8.0
2. Added PPPoE support
3. Added '#' as 'call' key option
4. Added 'Frames per TX' option for each codec
5. Added stutter dial tone for voice message indication
6. Added multiple line text message disply
7. In standby mode, press 'call' key to review missed call list
8. Added missed call indication on LCD
9. Let volume change display disappear in 3 seconds
10. Geisom <geisom at canall.com.br> added Brasil Portuguese support
11. Added Visual Studio 2008 support for SDCC\tool\ projects
12. Fixed a SIP caller ID bug
13. Added support for SIP "Replaces:" header


0.29 & 0.30 Revision list (Mar 27, 2008):
1. Alex Gradinar <Alex.Gradinar at cartel.md> added Russian and Romanian support, with extended chars input update
2. Added using 'Left' key as 'Cancel', 'Menu' key as 'OK' to navigate thru menu (by Alex)
3. Extended the size of INPUT html tag of phonebook and digitmap to 32 (by Alex)
4. Fixed a response code bug of SIP MESSAGE request
5. Fixed an auto-provision digitmap bug
6. Fixed a Spanish web page bug
7. Improved HOOK and Keypad detection
8. Added high-level UART communication protocol for AR168M VOIP module
9. Added simple IVR support for device without LCD
10. Fixed a day light saving bug


0.27 & 0.28 Revision list (Feb 25, 2008):
1. Added a new time zone in Venezuela, (GMT-04:30) Caracas
2. Added SetHighSpeed() function call in API
3. Added DTMF detect for both IP phone and ATA
4. Improved iLBC codec voice quality
5. Added multiple line text message display for IAX2
6. Improved LCD display speed
7. Added general ISO-8859-1/2/5/9 fonts support for all 2x16 and dot-matrix LCD display
8. Ferhat OLGUN <ferhatolgun2 at yahoo.com> added Turkish support
9. Improved keypad jitter handling


0.25 & 0.26 Revision list (Dec 7, 2007):
1. Added "MiniRun" method to handle incoming and outgoing RTP packets in time, solved jitter problem when handling SIP messages
2. Added iLBC 20ms frame support
3. Added Speex 8k sampling all 8 mode decode
4. General playback voice quality improvement for all codecs
5. Fixed a RTP timestamp bug when G.729 VAD enabled
6. Fixed false success message when using web to upgrade safemode file. Safemode file can only be upgraded by TFTP
7. Added memory dial DTMF send out for IAX2 call transfer function
8. Added UART control functions in software API, UART-poll serial spec and template provided by matthias.siegele AT baudisch.de
9. Added CE3 control functions in software API
10. Added SIP SUBSCRIBE for Message Waiting Indication
11. Added Subscribe for MWI configuration item on Web interface and menu interface


0.23 & 0.24 Revision list (Nov 9, 2007):
1. Improved echo cancellation quality
2. Added Speex 8k sampling 2k/4k/6k/8k/15kbps bitrate support
3. Added layer 2 (VLAN 802.3p&Q) and layer 3 QoS support
4. Added ip fragment support, now can support IP data up to 2960 bytes (ping 2952 bytes)
5. New "Manager" tool to replace PA1688 "Palmtool"
6. Fixed an ARP send repeat request bug
7. Fixed call time larger than 65536 seconds display bug
8. Fixed SIP DNS SRV bug
9. Fixed an UDP checksum bug
10. Fixed a SIP STUN bug
11. Moved call record from DSP memory to heap


0.21 & 0.22 Revision list (Oct 17, 2007):
* 1-4 changes by Alex Gradinar
1. Send syslog message without '\r'
2. Show SIP status code and Reason Phrase on LCD instead of "Peer rejected" for all SIP call failures
3. Show Calling number instead of Calling Name while incoming call
4. Show phone number after changing volume while talk
5. Diego Frick <dfrick at bit2net.com> added Spanish support
6. Fixed a timer bug, now SNTP is programmed to update with server every 18 hours
7. Improved overall LCD display performance
8. Change continue key press to only 1 valid key 
9. Fixed a GP1266 Mute LED bug
10. Added code limit check in hex2bin utility
11. Added New Zealand Daylight Saving time


0.19 & 0.20 Revision list (Sep 11, 2007):
1. Added calling through SIP proxy without registration
2. Added RFC 3164, the BSD syslog protocol
3. Fixed a SIP registration bug


0.17 & 0.18 Revision list (Aug 29, 2007):
1. Fixed a bug when using 4x20 character LCD
2. Added first version of factory test program
3. Added #3*0 DSP test
4. Alex Gradinar added 'Best Fit' algorithm in malloc function
5. Added G.729 Annex B VAD&CFG support
6. Improved TFTP performance
7. Fixed a input volume bug
8. Added side-tone, watch-dog and DSP MIPS control API functions


0.15 & 0.16 Revision list (Jun 22, 2008):
1. Dot-matrix design ascii font changed to iso 8859-1 standard
2. Added Moscow Daylight Saving time
3. Fixed delay1ms() function to delay accurate time
4. Fixed a RTP SSRC compare bug
5. Fixed a SIP to-TAG bug
6. Fixed a program flash erase failure problem
7. Fixed a SIP call hold bug
8. Added automatic hangup feature


0.13 & 0.14 Revision list (Jun 5, 2007):
1. Continue to improve voice quality for G.729 and iLBC, now G.729 passed all ITU-T bit-exactly test vectors
2. Fixed another out-going packet sequence bug, and "Voice Frames per TX" option is OK to use now
3. Enlarged all codec out-going packet buffer except for G.711, also enlarged hardware record buffer
4. Enlarged all codec in-coming packet buffer except for G.711
5. Added French accented characters display for dot-matrix LCD design like AR168G
6. Fixed a SIP call transfer bug
7. Fixed a local generate ringback tone bug
8. Fixed a ring bug
9. Updated SDCC compiler to 2.7.0 release version
10. Alfredo Sorbello <sorbello@antek.it> added Italian support
11. Can set "Auto Provisioning Server" as URL with subfolder
12. Hardware and software information is shown in HTML page
13. Fixed a DNS bug


0.12 Revision list (May 16, 2007):
1. Continue to improve voice quality for G.729 and iLBC
2. Fixed a out-going packet sequence bug
3. Fixed a dead-lock bug when DHCP failed
4. Watchdog is enabled by default from this version
5. Updated SDCC compiler to 2.7.0 release candidate 1
6. Sebastien RAILLARD <sebastien.raillard at gmail.com> added French support


0.11 Revision list (Apr 28, 2007):
1. Added G.729/G.729A support
2. Improved SIP message handling efficiency
3. Alex Gradinar <Alex.Gradinar at cartel.md> added Europe Daylight Saving time


0.10 Revision list (Apr 6, 2007):
1. Added hold music.
2. Adjust voice coder order.
3. Parameter "Register TTL" comes into effect.
4. Multiple SIP bugs fixed
5. Added SIP response 401 handling
6. Call history is displayed in time order. Get rid of deleting one call history entry and make it "Delete all"
7. Modify Web interface and menu.


0.09 Revision list (Mar 24, 2007):
1. Added iLBC codec
2. Ringtone size is cut down to half the original size
3. Optimize LCD display speed
4. Get rid of AR168A, AR168B, AR168C related code and compile options
5. Speed up hook down detection
6. Speed dial M1-M3 is statically mapped to entry 1-3 of phonebook
7. Added digitmap handling
8. Added IAX2 call forwarding function
9. Added SIP call waiting, call hold, call forwarding, call transfer
10. Added ringback tone
11. Added auto-answer
12. Added pressing "Mute/Dnd" to reject incoming call
13. Added call duration display
14. Added Hold LED display
15. Miscellanious LCD display information modified
