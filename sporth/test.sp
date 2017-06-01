_xm "./xm.so" fl

_clock _bpm _out "tempest-acidjazz.xm" _xm fe

0 _out tblsize _out ftsum

# feed into enveloped sine
# _clock get 0.001 0.001 0.001 tenvx 1000 0.5 sine * + 
_bpm get "+++2(++)" prop 0.001 0.003 0.001 tenvx 1000 0.5 sine * + 
_xm fc
-3 ampdb *
