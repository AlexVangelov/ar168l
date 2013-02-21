attrib -r /s /d

cd src

call mk.bat ar168g sip us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ar168g sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ar168m sip us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ar168m sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ar168r sip us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ar168r sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat gp1266 sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat gp1266 iax2 us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat gp2266 sip us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat gp2266 sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat gf302 sip us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat gf302 sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ywh201 sip us zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ywh201 sip cn zip
rem delay 2 seconds before next build
ping -n 3 localhost > nul

call mk.bat ar168r sip us uart
rem delay 2 seconds before next build
ping -n 3 localhost > nul

rem call mk.bat ar168r sip us remota

cd..
