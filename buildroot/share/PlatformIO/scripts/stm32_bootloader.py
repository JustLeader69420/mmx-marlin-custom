import os,sys,shutil
Import("env")

from SCons.Script import DefaultEnvironment
board = DefaultEnvironment().BoardConfig()

def noencrypt(source, target, env):
  firmware = os.path.join(target[0].dir.path, board.get("build.firmware"))
  # do not overwrite encrypted firmware if present
  if not os.path.isfile(firmware):
    shutil.copy(target[0].path, firmware)

print('dashu: stm32 bootloader py execute')

# for define in env['CPPDEFINES']:
#   LD_FLASH_OFFSET = ''
#   if define[0] == "VECT_TAB_OFFSET":
#     LD_FLASH_OFFSET = define[1]
#     print('langgo: find define.vect tab offset:' + LD_FLASH_OFFSET)
#   maximum_ram_size = board.get("upload.maximum_ram_size")

# if 'offset' in board.get("build").key():
#   LD_FLASH_OFFSET = board.get("build.offset")
#   print('dashu: find define.vect tab offset:' + LD_FLASH_OFFSET)
#   maximum_ram_size = board.get("upload.maximum_ram_size")

#   for i, flag in enumerate(env["LINKFLAGS"]):
#     if "-Wl,--defsym=LD_FLASH_OFFSET" in flag:
#       env["LINKFLAGS"][i] = "-Wl,--defsym=LD_FLASH_OFFSET=" + LD_FLASH_OFFSET
#     if "-Wl,--defsym=LD_MAX_DATA_SIZE" in flag:
#       env["LINKFLAGS"][i] = "-Wl,--defsym=LD_MAX_DATA_SIZE=" + str(maximum_ram_size - 40)

  # if 'firmware' in board.get("build").keys():
  #   env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", noencrypt);

# 'offset' property defined in buildroot\share\PlatformIO\boards\langgo407ve.json
if 'offset' in board.get("build").keys():
  LD_FLASH_OFFSET = board.get("build.offset")

  to_remove = []
  to_append = []
  for define in env['CPPDEFINES']:
    if define[0] == "VECT_TAB_OFFSET":
      to_remove.append(define)
      to_append.append(("VECT_TAB_OFFSET", LD_FLASH_OFFSET))
  for define in to_remove:
    env['CPPDEFINES'].remove(define)
  for define in to_append:
    env['CPPDEFINES'].append(define)
  # for define in env['CPPDEFINES']:
  #   if define[0] == "VECT_TAB_OFFSET":
  #     env['CPPDEFINES'].remove(define)
  # env['CPPDEFINES'].append(("VECT_TAB_OFFSET", LD_FLASH_OFFSET))
  print('dashu: vect tab offset' + LD_FLASH_OFFSET)

  maximum_ram_size = board.get("upload.maximum_ram_size")

  for i, flag in enumerate(env["LINKFLAGS"]):
    if "-Wl,--defsym=LD_FLASH_OFFSET" in flag:
      env["LINKFLAGS"][i] = "-Wl,--defsym=LD_FLASH_OFFSET=" + LD_FLASH_OFFSET
    if "-Wl,--defsym=LD_MAX_DATA_SIZE" in flag:
      env["LINKFLAGS"][i] = "-Wl,--defsym=LD_MAX_DATA_SIZE=" + str(maximum_ram_size - 40)

  if 'firmware' in board.get("build").keys():
    env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", noencrypt);
