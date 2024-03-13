/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#define REVERSE_TEMP_SENSOR_RANGE_1047 1

#define Pt1000AdVal(T,R0,Rup) (short)(33000 / (Rup / PtRt(T, R0) + 1))
#define Pt1000Line(T,R0,Rup)  { Pt1000AdVal(T, R0, Rup), T }
// Pt1000 with 4k7 pullup
const temp_entry_t temptable_1047[] PROGMEM = {
  // only a few values are needed as the curve is very flat
  Pt1000Line(  0, 1000, 4700),
  Pt1000Line( 50, 1000, 4700),
  Pt1000Line(100, 1000, 4700),
  Pt1000Line(150, 1000, 4700),
  Pt1000Line(200, 1000, 4700),
  Pt1000Line(250, 1000, 4700),
  Pt1000Line(300, 1000, 4700),
  Pt1000Line(350, 1000, 4700),
  Pt1000Line(400, 1000, 4700),
  Pt1000Line(450, 1000, 4700),
  Pt1000Line(500, 1000, 4700)
};
