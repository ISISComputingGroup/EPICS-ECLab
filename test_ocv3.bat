setlocal

if "%MYPVPREFIX%" == "" (
    set MYPVPREFIX=TE:NDW1548:
)

REM set prefix for files created by technique - directory plus start of filename
caput -S %MYPVPREFIX%ECLAB_01:C0:FILEPREFIX:SP "c:/data/eclab"

REM set parameters for OCV technique
REM Format of technique PV is C{channel}:T:{technique}:{index}:{param}:SP
REM index is 0 unless the technique will be specified more than once in the LOADTECH command

REM this sets parameter RTT (rest time T) for technique OCV on channel 0
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:RTT:SP 10
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:REDE:SP 1
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:REDT:SP 1
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:TB:SP 20e-6
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:IRANGE:SP 8
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:ERANGE:SP 3
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:0:BW:SP 5

caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:RTT:SP 10
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:REDE:SP 1
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:REDT:SP 0.5
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:TB:SP 20e-6
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:IRANGE:SP 8
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:ERANGE:SP 3
caput %MYPVPREFIX%ECLAB_01:C0:T:OCV:1:BW:SP 5

caput %MYPVPREFIX%ECLAB_01:C0:T:LOOP:0:NLOOP:SP 2
caput %MYPVPREFIX%ECLAB_01:C0:T:LOOP:0:PROT:SP 0

REM after setting parameters, load technique on channel 
REM you cannot start until you have loaded a technique, and if you 
REM stop you need to do LOADTECH again before another start
REM technique name in lowercase
caput %MYPVPREFIX%ECLAB_01:C0:LOADTECH:SP "ocv,ocv,loop"
 
REM now start the channel
REM this opens the data file
caput %MYPVPREFIX%ECLAB_01:C0:START 1

REM check status of channel
caget %MYPVPREFIX%ECLAB_01:C0:STATE

REM other current values 
caget %MYPVPREFIX%ECLAB_01:C0:EWE
caget %MYPVPREFIX%ECLAB_01:C0:ECE
caget %MYPVPREFIX%ECLAB_01:C0:I

pause

REM now stop the channel
REM this closes the data file
caput %MYPVPREFIX%ECLAB_01:C0:STOP 1

