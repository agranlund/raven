-------------------------------------------------------------------------------
  ISA floppy disk driver
  (c)2026 Anders Granlund
-------------------------------------------------------------------------------
 This file is free software  you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation  either version 2, or (at your option)
 any later version.

 This file is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY  without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program  if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
-----------------------------------------------------------------------------


**** This driver is beta, and with READ-ONLY floppy support ****


usage:
 Put FDC.PRG in auto folder or run it manually.
 It will install as the first free floppy drive, either A or B.

Compatibility:
 It should be compatible with any PD765 compatible
 controller that lives on port 3F0
