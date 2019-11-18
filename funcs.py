import genie_python.genie as g

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
g.set_pv("ECLAB_01:C0:T:OCV:0:XCTR:SP", 0, is_local=True)

# set second ocv
g.set_pv("ECLAB_01:C0:T:OCV:1:RTT:SP", 10, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:REDE:SP", 1, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:REDT:SP", 1, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:TB:SP", 20e-6, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:IRANGE:SP", 8, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:ERANGE:SP", 3, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:BW:SP", 5, is_local=True)
g.set_pv("ECLAB_01:C0:T:OCV:1:XCTR:SP", 0, is_local=True)

vsinit = [ 1, 1, 1 ]
vstep = [ 1.0, 1.0, 1.0 ]
durstep = [ 10.0, 11.0, 12.0 ]
stepn = len(vstep) - 1

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
g.set_pv("ECLAB_01:C0:T:CP:0:XCTR:SP", 0, is_local=True)

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
g.set_pv("ECLAB_01:C0:T:CA:0:XCTR:SP", 0, is_local=True)

#g.set_pv("ECLAB_01:C0:UPDATE:SP", 1, is_local=True)

g.set_pv("ECLAB_01:C0:LOADTECH:SP", "ocv,ca,cp,ocv", is_local=True)
 
g.set_pv("ECLAB_01:C0:START", 1, is_local=True)
