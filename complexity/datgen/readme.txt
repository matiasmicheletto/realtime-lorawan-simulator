NAME
   datgen - ".dat" file generator

SYNOPSIS
   datgen [OPTION]... -i [FILE] > instance.dat

DESCRIPTION:
   This program generates instances of the Gateway Placement Problem (GPP) to be solved using the gpprs program.


OPTIONS:
   -h, --help     Display this help message.
   -i, --input    Input file with instance parameters.

EXAMPLES:
   1. datgen -i input.gen
      - load input.gen with instance parameters.

AUTHOR
   Written by Matias J. Micheletto.

REPORTING BUGS
   Follow guidelines from <https://github.com/matiasmicheletto/realtime-lorawan-simulator>.

COPYRIGHT
   Copyright   ©   2023   Free   Software   Foundation,  Inc.   License  GPLv3+:  GNU  GPL  version  3  or  later <https://gnu.org/licenses/gpl.html>.
   This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the  extent  permitted by law.
