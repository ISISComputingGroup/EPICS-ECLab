from genie_python import genie as g

g.set_instrument(None)

# set filename
g.set_pv("ECLAB_01:C0:FILEPREFIX:SP", "c:/data/eclab", is_local=True)

# set first ocv
g.set_pv("ECLAB_01:C0:T:OCV:0:RTT:SP", 10, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:REDE:SP", 1, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:REDT:SP", 1, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:TB:SP", 20e-6, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:IRANGE:SP", 8, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:ERANGE:SP", 3, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:BW:SP", 5, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:0:XCTR:SP", 0, is_local=True) # 1

# set second ocv
g.set_pv("ECLAB_01:C0:T:OCV:1:RTT:SP", 10, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:REDE:SP", 1, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:REDT:SP", 1, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:TB:SP", 20e-6, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:IRANGE:SP", 8, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:ERANGE:SP", 3, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:BW:SP", 5, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:XCTR:SP", 0, is_local=True) # 1

vsinit = [ 1, 1, 1 ]
istep = [ 1.0, 1.0, 1.0 ]
durstep = [ 10.0, 11.0, 12.0 ]
stepn = len(istep) - 1

g.set_pv("ECLAB_01:C0:T:CP:0:REDT:SP", 0.1, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:REDE:SP", 0.1, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:TB:SP", 25e-6, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:VSINIT:SP", vsinit, is_local=True) 
g.set_pv("ECLAB_01:C0:T:CP:0:ISTEP:SP", istep, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:DURSTEP:SP", durstep, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:IRANGE:SP", 8, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:ERANGE:SP", 3, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:BW:SP", 5, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:STEPN:SP", stepn, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:NCYCLES:SP", 0, is_local=True)
g.set_pv("ECLAB_01:C0:T:CP:0:XCTR:SP", 0, is_local=True) # 1 + 32 + 64

vsinit = [ 1, 1, 1 ]
vstep = [ 1.0, 1.0, 1.0 ]
durstep = [ 10.0, 11.0, 12.0 ]
stepn = len(vstep) - 1

g.set_pv("ECLAB_01:C0:T:CA:0:REDT:SP", 0.1, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:REDI:SP", 0.1, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:TB:SP", 25e-6, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:VSINIT:SP", vsinit, is_local=True) 
g.set_pv("ECLAB_01:C0:T:CA:0:VSTEP:SP", istep, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:DURSTEP:SP", durstep, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:IRANGE:SP", 8, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:ERANGE:SP", 3, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:BW:SP", 5, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:STEPN:SP", stepn, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:NCYCLES:SP", 0, is_local=True)
g.set_pv("ECLAB_01:C0:T:CA:0:XCTR:SP", 0, is_local=True) # 1 + 32 + 64

# length number of steps

g.set_pv("ECLAB_01:C0:T:CALIMIT:0:TB:SP", 34e-6, is_local=True)
# default E (potential), +96 for I (current) 
# default <= , +4 for >=
# default OR between tests, +2 for AND
# default not active, +1 for active 
test1conf = [ ] 
exitcond = [ ] # 0 = next step, 1=next technique, 2=stop experiment
test1val = [ ]
g.set_pv("ECLAB_01:C0:T:CALIMIT:0:TEST1CONF:SP", test1conf, is_local=True)
g.set_pv("ECLAB_01:C0:T:CALIMIT:0:TEST1VAL:SP", test1val, is_local=True)
g.set_pv("ECLAB_01:C0:T:CALIMIT:0:EXITCOND:SP", exitcond, is_local=True)

#g.set_pv("ECLAB_01:C0:UPDATE:SP", 1, is_local=True)

g.set_pv("ECLAB_01:C0:LOADTECH:SP", "ocv,ca,cp,ocv", is_local=True)
 
g.set_pv("ECLAB_01:C0:START", 1, is_local=True)
