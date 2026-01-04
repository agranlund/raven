ISA plug and play bios for Atari compatibles

Main development platform is Raven but it has built-in support
for other ISA enabled Atari platforms too.


- Raven
  Main developement platform.
  IRQ is supported.
  DMA not supported by the hardware.

- Hades
  (limited support. IRQ support not implemented)

- Milan
  (limited support. IRQ support not implemented)

- Panther/2
  (limited support. IRQ support not implemented)

- Others
  Limited support for other platforms can be added by
  specifying the bus configuration in isa_bios.inf





Portions of this software is based on code from FreeBSD.

----------------------------------------------------------------------------
SPDX-License-Identifier: BSD-2-Clause-FreeBSD

Copyright (c) 1996, Sujal M. Patel
Copyright (c) 2025, Anders Granlund
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
----------------------------------------------------------------------------

