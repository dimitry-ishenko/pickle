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

#FreeBSD IBMPC: FreeBSD 9.0-RELEASE   amd64
#Debian  IBMPC: Linux   3.2.0-4-amd64 x86_64
#Debian  RPi:   Linux   3.2.27+       armv6l
SYSTEM:=$(shell uname -s)
RELEASE:=$(shell uname -r)
PLATFORM:=$(shell uname -m)

linux:
	cd src && $(MAKE) CONFIG=config/linux.mk

k8048:
	cd src && $(MAKE) CONFIG=config/K8048.mk

cygwin:k8048

freebsd:k8048

linux-install:
	cd src && $(MAKE) CONFIG=config/linux.mk install

k8048-install:
	cd src && $(MAKE) CONFIG=config/K8048.mk install

cygwin-install:
	cd src && $(MAKE) CONFIG=config/K8048.mk BINDIR=$(HOME)/bin BINUSER=$(USER) BINGROUP=$(USER) install

freebsd-install:k8048-install

install:linux-install

uninstall:
	cd src && $(MAKE) uninstall

cygwin-uninstall:
	cd src && $(MAKE) BINDIR=$(BINDIR) uninstall

clean:
	cd src && $(MAKE) clean
	find -name "*~" -delete

rpibin:
ifneq "$(PLATFORM)" "armv6l"
	$(error incorrect platform)
endif
	cd src && sudo $(MAKE) CONFIG=config/rpibin.mk rpibin

ls:
	find -type f -not -path "./.hg/*" -ls
