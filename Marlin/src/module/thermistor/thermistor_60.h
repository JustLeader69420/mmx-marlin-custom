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

// R25 = 100 kOhm, beta25 = 3950 K, 4.7 kOhm pull-up,
// Maker's Tool Works Kapton Bed Thermistor
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=3950
// r0: 100000
// t0: 25
// r1: 0 (parallel with rTherm)
// r2: 4700 (series with rTherm)
// beta: 3950
// min adc: 1 at 0.0048828125 V
// max adc: 1023 at 4.9951171875 V
const temp_entry_t temptable_60[] PROGMEM = {
    { OV( 20 ),  356  },
    { OV( 30 ),  317  },
    { OV( 40 ),  292  },
    { OV( 50 ),  273  },
    { OV( 60 ),  259  },
    { OV( 70 ),  248  },
    { OV( 80 ),  238  },
    { OV( 90 ),  229  },
    { OV( 100 ),  222  },
    { OV( 110 ),  216  },
    { OV( 120 ),  210  },
    { OV( 130 ),  204  },
    { OV( 140 ),  200  },
    { OV( 150 ),  195  },
    { OV( 160 ),  191  },
    { OV( 170 ),  187  },
    { OV( 180 ),  183  },
    { OV( 190 ),  180  },
    { OV( 200 ),  176  },
    { OV( 210 ),  173  },
    { OV( 220 ),  170  },
    { OV( 230 ),  168  },
    { OV( 240 ),  165  },
    { OV( 250 ),  162  },
    { OV( 260 ),  160  },
    { OV( 270 ),  157  },
    { OV( 280 ),  155  },
    { OV( 290 ),  153  },
    { OV( 300 ),  151  },
    { OV( 310 ),  149  },
    { OV( 320 ),  146  },
    { OV( 330 ),  144  },
    { OV( 340 ),  143  },
    { OV( 350 ),  141  },
    { OV( 360 ),  139  },
    { OV( 370 ),  137  },
    { OV( 380 ),  135  },
    { OV( 390 ),  133  },
    { OV( 400 ),  132  },
    { OV( 410 ),  130  },
    { OV( 420 ),  128  },
    { OV( 430 ),  127  },
    { OV( 440 ),  125  },
    { OV( 450 ),  123  },
    { OV( 460 ),  122  },
    { OV( 470 ),  120  },
    { OV( 480 ),  119  },
    { OV( 490 ),  117  },
    { OV( 500 ),  116  },
    { OV( 510 ),  114  },
    { OV( 520 ),  113  },
    { OV( 530 ),  111  },
    { OV( 540 ),  110  },
    { OV( 550 ),  108  },
    { OV( 560 ),  107  },
    { OV( 570 ),  105  },
    { OV( 580 ),  104  },
    { OV( 590 ),  103  },
    { OV( 600 ),  101  },
    { OV( 610 ),  100  },
    { OV( 620 ),  98  },
    { OV( 630 ),  97  },
    { OV( 640 ),  95  },
    { OV( 650 ),  94  },
    { OV( 660 ),  93  },
    { OV( 670 ),  91  },
    { OV( 680 ),  90  },
    { OV( 690 ),  88  },
    { OV( 700 ),  87  },
    { OV( 710 ),  85  },
    { OV( 720 ),  84  },
    { OV( 730 ),  82  },
    { OV( 740 ),  81  },
    { OV( 750 ),  79  },
    { OV( 760 ),  78  },
    { OV( 770 ),  76  },
    { OV( 780 ),  74  },
    { OV( 790 ),  73  },
    { OV( 800 ),  71  },
    { OV( 810 ),  69  },
    { OV( 820 ),  67  },
    { OV( 830 ),  66  },
    { OV( 840 ),  64  },
    { OV( 850 ),  62  },
    { OV( 860 ),  60  },
    { OV( 870 ),  58  },
    { OV( 880 ),  55  },
    { OV( 890 ),  53  },
    { OV( 900 ),  51  },
    { OV( 910 ),  48  },
    { OV( 920 ),  46  },
    { OV( 930 ),  43  },
    { OV( 940 ),  40  },
    { OV( 950 ),  36  },
    { OV( 960 ),  33  },
    { OV( 970 ),  28  },
    { OV( 980 ),  23  },
    { OV( 990 ),  18  },
    { OV( 1000 ),  10  },
    { OV( 1010 ),  0  },
    // { OV( 1020 ),  -22  },
};
