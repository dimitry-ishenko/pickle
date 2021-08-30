#
# Copyright (C) 2005-2020 Darron Broad
# All rights reserved.
#
# This file is part of Pickle Microchip PIC ICSP.
#
# Pickle Microchip PIC ICSP is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation.
#
# Pickle Microchip PIC ICSP is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with Pickle Microchip PIC ICSP. If not, see http://www.gnu.org/licenses/
#

# Velleman K8048 for FreeBSD, CYGWIN etc

# Build targets (enable:1 disable:0)
P12=1
P14=1
N14=1
P16=1
N16=1
P24=0
P32=0
PCTRL=0
PTEST=0
PIO=0
PLOAD=0

# Frontend Inputs (enable:1 disable:0)
STK500=0

# Backend Outputs (where available) (enable:1 disable:0)
RPI=0
ALLWINNER=0
MCP23016=0
MCP230XX=0
MCP23SXX=0
PCF8574=0
BITBANG=0
FTDI=0
MCP2221=0
CP2104=0
SERIAL=1

# gdb else strip (gdb:1 strip:0)
GDB=0

# output verbose config (enable:1 disable:0)
VERBOSE=1

# output boot loader config (enable:1 disable:0)
LOADER=0

# use QSORT (quick sort) else TSEARCH (tree sort) (qsort:1 tsearch:0)
QSORT=1
