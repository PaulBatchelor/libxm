_xm "./xm.so" fl

_tick _clock "milky.xm" _xm fe

0 _out tblsize _out ftsum

# feed into enveloped sine
_clock get 0.001 0.001 0.001 tenvx 1000 0.5 sine * + 
_xm fc
