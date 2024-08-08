# Modified Mingda Magician X firmware :)

These are my edits of the MMX firmware, enjoy.

## Support:
 - If you'd like to get help, suggest changes, or just generally discuss these modifications, you can join my Discord server here: https://discord.gg/e9fndmGUsY :)

## Edits and fixes:
 - Icons in print menu centered (moved one icon to the right)
 - PID_FUNCTIONAL_RANGE increased, as I had overshoot problems with the alloy hotend. (Eh, it still overshoots, but I don't really care anymore lol)
 - Languages added (Uncommented and fixed up) - cz, sk, cz2 (Czech, but more professional/literal/better translations, cz is kinda fun compared to this one)
   - changes in Marlin/src/lcd/extui/lib/tsc/Language/ to add a language
      - Language has to be included in Language.cpp
         - Check line 2648, which links to the language pack, which you create in Language.cpp too, copy a pack and change the language code before all the variables, vscode has got you covered, you can select all the variables and in the Ctrl+F find menu, you can tell it to replace within selection.
         - You then need to create the language file, it should be in the Language folder with all the other ones, and included in Language.cpp at the top.
      - You have to add it to the menu, see file Language.h from line 6, this enum list also determines the order the languages are cycled in the language settings button.
 - Status information moved to top of screen next to ready message to free up 2 icon spaces (toggleable, see Marlin/Configuration.h)
 - Added CFW version to info menu
 - Changed long touch time in touch_process.cpp to not have to wait 3-5 business days in order to delete a file (not sure where else this delay might be used)
 - Added custom functions menu
   - Find under main status screen > Functions, or if using old status screen, under main status screen > Menu > Settings > Functions
   - Added button to resume print after M600 pause when not printing from inserted media (ex. OctoPrint/USB)
   - Added button to calibrate touchscreen without having to create the reset.txt file
 - Because of the menu modifications, a few icons have been enabled, which previously only existed in the TFT35 folder, but weren't used when updating icons, if you see empty icons, download a TFT35 theme folder from either Mingda, or ![my themes repository](https://git.justleader.eu/justleader/mmx-marlin-themes), then install them.
 - Make Preheat filament types changeable (for now only in sourcecode before building) in Configuration.h
 - \+ there might be a few more changes, I try to add them to the list when they are finished
 
## Notes:
 - The firmware is by default set for the alloy hotend
    - To change the nozzle temperature limit back from 315 to 275°C, comment out line 41 in Marlin/Configuration.h
    ```
    #define USE_TI_CHOKE        // 加入钛合金喉管
    ```
    - Alternatively, to change the temperature limit, check lines 545 to 549 in Marlin/Configuration.h out:
    ```
    #ifdef USE_TI_CHOKE
    #define HEATER_0_MAXTEMP 315
    #else
    #define HEATER_0_MAXTEMP 275
    #endif
    ```
 - The firmwares in the ![releases](https://git.justleader.eu/justleader/mmx-marlin-custom/releases/) page have tags:
    - xx-yy where xx is the GD or ST version and yy is either ti for alloy hotend, or nm for normal
    - the tag can also be "multiple", if there are multiple types, in these cases, the files won't be named firmware.bin, but by their versions, as with the tags (xx-yy.bin), these must be renamed to firmware.bin before updating, or the printer will not acknowledge them.
 - The files for the GUI of the TFT screen are located at Marlin/src/lcd/extui/lib/tsc/Menu
 
## Installation:
 - Build the source, or download a build from the ![releases](https://git.justleader.eu/justleader/mmx-marlin-custom/releases/) page
 - If the .bin file isn't named firmware.bin already, rename it
 - Put the .bin file in the lowest folder of your portable storage
 - Put the storage into the printer and turn the printer on
 - Wait for the printer to update, you don't have to delete the firmware afterwards, as it gets renamed, but you can still delete the file after the update.
 - If your printer gets stuck at 100%, just leave it for a minute, then unplug the power and plug it back in. The printer should still be updated.
 - Done! You can now go check out some ![themes](https://git.jldr.eu/justleader/mmx-marlin-themes) too :)
   - Consider installing an icon pack (can be the default one) when switching from the vanilla version, as there are unused icons in the vanilla version, that I use, and that don't get uploaded to the machine when updating icons with the vanilla firmware.

## Building
 - build the same as a normal Marlin firmware, I use vscode with the PlatformIO extension, from where you can build the project using the "PlatformIO: Build" button in the bottom buttons list
 - to change the firmware type (GD or ST), comment the unsuitable type in platformio.ini, line 21 and 22, for example to enable GD:
 ```
 default_envs = langgo407ve_gd
; default_envs = langgo407ve_st
 ```
 
 
## Other customization
 - Ready message: line 210 in Marlin/Configuration.h
   ```
     #define CUSTOM_MACHINE_NAME "MD D301 F4"
   ```
- Changing version:
  - Version information is changed in Marlin/src/inc/Version.h
- Reverting to old main status screen
  - Uncomment `#define USEOLDSTATUSSCREEN` at the top in Marlin/Configuration.h, custom settings are at the top, before the Marlin configuration

## Current issues
 - None that I know of

## TODO
 - Try to fix weird font when Czech language
   - Change order of fonts in utf8_decode.cpp?
 - Add status message to in-print menu
 - Screen auto off timer?
   - probably located in Menu/Settings/Other settings/
 - Temperature, Fan speed lock buttons to prevent GCODE changing custom settings
 - "Start with screen on/off" button
 - Add confirmation to Leveling button (missclick when printing from octoprint, pisses me off that it bypasses octoprint)
 - when USEOLDSTATUSSCREEN, put Functions menu to a new Other settings menu, where we can put more settings, maybe multiple pages, or a settings list?
 - Make more modifications undoable?
   - Centering print menu icons
 - Add reset settings button somewhere
 - GCODE to resume print from terminal?
 - Add GCODE to manipulate all the settings currently only available on the touchscreen?
 - Make the language button open a list instead of cycling?
 - Skip storage selection menu when only one storage media type inserted?
   - show error when no media inserted and return to main menu
 - https://github.com/MINGDA3D/KlipperScreen style menu? 👀
 - Look at some wifi connection options?
 - overhaul click handling (Marlin/src/lcd/extui/lib/tsc/Hal/touch_process.cpp)
 - respect silent mode setting when beeping
 - Custom GCODE to change all menu settings
   - also things like preheat filament types (custom filament name)


# Original README.md

# Marlin 3D Printer Firmware

![GitHub](https://img.shields.io/github/license/marlinfirmware/marlin.svg)
![GitHub contributors](https://img.shields.io/github/contributors/marlinfirmware/marlin.svg)
![GitHub Release Date](https://img.shields.io/github/release-date/marlinfirmware/marlin.svg)
[![Build Status](https://github.com/MarlinFirmware/Marlin/workflows/CI/badge.svg?branch=bugfix-2.0.x)](https://github.com/MarlinFirmware/Marlin/actions)

<img align="right" width=175 src="buildroot/share/pixmaps/logo/marlin-250.png" />

Additional documentation can be found at the [Marlin Home Page](https://marlinfw.org/).
Please test this firmware and let us know if it misbehaves in any way. Volunteers are standing by!

## Marlin 2.0

Marlin 2.0 takes this popular RepRap firmware to the next level by adding support for much faster 32-bit and ARM-based boards while improving support for 8-bit AVR boards. Read about Marlin's decision to use a "Hardware Abstraction Layer" below.

Download earlier versions of Marlin on the [Releases page](https://github.com/MarlinFirmware/Marlin/releases).

## Building Marlin 2.0

To build Marlin 2.0 you'll need [Arduino IDE 1.8.8 or newer](https://www.arduino.cc/en/main/software) or [PlatformIO](http://docs.platformio.org/en/latest/ide.html#platformio-ide). Detailed build and install instructions are posted at:

  - [Installing Marlin (Arduino)](http://marlinfw.org/docs/basics/install_arduino.html)
  - [Installing Marlin (VSCode)](http://marlinfw.org/docs/basics/install_platformio_vscode.html).

### Supported Platforms

  Platform|MCU|Example Boards
  --------|---|-------
  [Arduino AVR](https://www.arduino.cc/)|ATmega|RAMPS, Melzi, RAMBo
  [Teensy++ 2.0](http://www.microchip.com/wwwproducts/en/AT90USB1286)|AT90USB1286|Printrboard
  [Arduino Due](https://www.arduino.cc/en/Guide/ArduinoDue)|SAM3X8E|RAMPS-FD, RADDS, RAMPS4DUE
  [LPC1768](http://www.nxp.com/products/microcontrollers-and-processors/arm-based-processors-and-mcus/lpc-cortex-m-mcus/lpc1700-cortex-m3/512kb-flash-64kb-sram-ethernet-usb-lqfp100-package:LPC1768FBD100)|ARM® Cortex-M3|MKS SBASE, Re-ARM, Selena Compact
  [LPC1769](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/lpc1700-cortex-m3/512kb-flash-64kb-sram-ethernet-usb-lqfp100-package:LPC1769FBD100)|ARM® Cortex-M3|Smoothieboard, Azteeg X5 mini, TH3D EZBoard
  [STM32F103](https://www.st.com/en/microcontrollers-microprocessors/stm32f103.html)|ARM® Cortex-M3|Malyan M200, GTM32 Pro, MKS Robin, BTT SKR Mini
  [STM32F401](https://www.st.com/en/microcontrollers-microprocessors/stm32f401.html)|ARM® Cortex-M4|ARMED, Rumba32, SKR Pro, Lerdge, FYSETC S6
  [STM32F7x6](https://www.st.com/en/microcontrollers-microprocessors/stm32f7x6.html)|ARM® Cortex-M7|The Borg, RemRam V1
  [SAMD51P20A](https://www.adafruit.com/product/4064)|ARM® Cortex-M4|Adafruit Grand Central M4
  [Teensy 3.5](https://www.pjrc.com/store/teensy35.html)|ARM® Cortex-M4|
  [Teensy 3.6](https://www.pjrc.com/store/teensy36.html)|ARM® Cortex-M4|
  [Teensy 4.0](https://www.pjrc.com/store/teensy40.html)|ARM® Cortex-M7|
  [Teensy 4.1](https://www.pjrc.com/store/teensy41.html)|ARM® Cortex-M7|

## Submitting Changes

- Submit **Bug Fixes** as Pull Requests to the ([bugfix-2.0.x](https://github.com/MarlinFirmware/Marlin/tree/bugfix-2.0.x)) branch.
- Follow the [Coding Standards](http://marlinfw.org/docs/development/coding_standards.html) to gain points with the maintainers.
- Please submit your questions and concerns to the [Issue Queue](https://github.com/MarlinFirmware/Marlin/issues).

## Marlin Support

For best results getting help with configuration and troubleshooting, please use the following resources:

- [Marlin Documentation](http://marlinfw.org) - Official Marlin documentation
- [Marlin Discord](https://discord.gg/n5NJ59y) - Discuss issues with Marlin users and developers
- Facebook Group ["Marlin Firmware"](https://www.facebook.com/groups/1049718498464482/)
- RepRap.org [Marlin Forum](http://forums.reprap.org/list.php?415)
- [Tom's 3D Forums](https://forum.toms3d.org/)
- Facebook Group ["Marlin Firmware for 3D Printers"](https://www.facebook.com/groups/3Dtechtalk/)
- [Marlin Configuration](https://www.youtube.com/results?search_query=marlin+configuration) on YouTube

## Credits

The current Marlin dev team consists of:

 - Scott Lahteine [[@thinkyhead](https://github.com/thinkyhead)] - USA &nbsp; [Donate](http://www.thinkyhead.com/donate-to-marlin)
 - Roxanne Neufeld [[@Roxy-3D](https://github.com/Roxy-3D)] - USA
 - Chris Pepper [[@p3p](https://github.com/p3p)] - UK
 - Bob Kuhn [[@Bob-the-Kuhn](https://github.com/Bob-the-Kuhn)] - USA
 - Erik van der Zalm [[@ErikZalm](https://github.com/ErikZalm)] - Netherlands &nbsp; [![Flattr Erik](https://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=ErikZalm&url=https://github.com/MarlinFirmware/Marlin&title=Marlin&language=&tags=github&category=software)

## License

Marlin is published under the [GPL license](/LICENSE) because we believe in open development. The GPL comes with both rights and obligations. Whether you use Marlin firmware as the driver for your open or closed-source product, you must keep Marlin open, and you must provide your compatible Marlin source code to end users upon request. The most straightforward way to comply with the Marlin license is to make a fork of Marlin on Github, perform your modifications, and direct users to your modified fork.

While we can't prevent the use of this code in products (3D printers, CNC, etc.) that are closed source or crippled by a patent, we would prefer that you choose another firmware or, better yet, make your own.
