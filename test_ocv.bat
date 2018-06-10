setlocal

if "%MYPVPREFIX%" == "" (
    set MYPVPREFIX=TE:NDW1548:
)

REM set prefix for files created by technique - directory plus start of filename
caput -S %MYPVPREFIX%ECLAB:FILEPREFIX:SP "c:/data/eclab"

REM set parameters for OCV technique
REM Format of technique PV is C{channel}:T:{technique}:{index}:{param}:SP

REM this sets parameter RTT (rest time T) for technique OCV on channel 0
caput %MYPVPREFIX%ECLAB:C0:T:OCV:0:RTT:SP 30
 
REM after setting parameters, load technique on channel 
REM you cannot start until you have loaded a technique, and if you 
REM stop you need to do LOADTECH again before another start
REM technique name in lowercase
caput %MYPVPREFIX%ECLAB:C0:LOADTECH:SP "ocv"
 
REM now start the channel
REM this opens the data file
caput %MYPVPREFIX%ECLAB:C0:START 1

REM check status of channel
caget %MYPVPREFIX%ECLAB:C0:STATE

REM other current values 
caget %MYPVPREFIX%ECLAB:C0:EWE
caget %MYPVPREFIX%ECLAB:C0:ECE
caget %MYPVPREFIX%ECLAB:C0:I

pause

REM now stop the channel
REM this closes the data file
caput %MYPVPREFIX%ECLAB:C0:STOP 1

