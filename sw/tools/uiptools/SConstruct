import os
import SCons.Builder
from os.path import expanduser

def setupToolchain(targetEnv):

    PREFIXES =  {
        '/usr/bin/' : 'm68k-atari-mint',
        '/opt/cross-mint/bin/' : 'm68k-atari-mint',
        expanduser("~/gnu-tools/m68000/bin/"): 'm68k-atari-mint',
        expanduser("/opt/m68k-ataritos/aout/bin/"): 'm68k-atari-mint',
        }

    CROSS_PREFIX = None

    for m68kPrefix in PREFIXES:
        if os.path.exists (  m68kPrefix + PREFIXES[m68kPrefix] + "-gcc" ) == True:
            CROSS_PREFIX = m68kPrefix + PREFIXES[m68kPrefix]
            break

    if CROSS_PREFIX == None:
        print("Error: Couldn't find atari toolchain.")
        Exit(1)

    print("Building with: " + CROSS_PREFIX)

    targetEnv["CC"] = CROSS_PREFIX + '-gcc'
    targetEnv["CXX"] = CROSS_PREFIX + '-g++'
    targetEnv["AS"] = CROSS_PREFIX + '-as'
    targetEnv["MINTFLAGS"] = CROSS_PREFIX + '-flags'
    targetEnv['CCFLAGS'] = '-m68000 -O3 -std=gnu99 -fomit-frame-pointer -ffast-math -I${TARGET.dir} '
    targetEnv['ASFLAGS'] = '-m68000'
    targetEnv['LINKFLAGS'] = '-m68000 -O3 -s '

    return targetEnv

def detectLibCMini(targetEnv):
    libcminiPath = os.environ.get('LIBCMINI')
    if libcminiPath:
        print ("Using libcmini in: " + libcminiPath)
        targetEnv.Append(LIBS=['cmini', 'gcc'])
        targetEnv.Append(LIBPATH=[os.path.abspath(libcminiPath)])
        targetEnv.Append(CCFLAGS='-nostdlib ')
        for startupFile in ['crt0.o', 'startup.o']:
            if FindFile(startupFile, libcminiPath):
                targetEnv.Append(LINKFLAGS='-nostdlib' + ' ' + libcminiPath + '/' + startupFile)
                break
        targetEnv.Append(CPPPATH=libcminiPath + '/../include')
    else:
        print("Libcmini not found, using default libs.")

def compressProgramMaybe(env, target):
    if not os.environ.get('NOUPX'):
        upx = env.WhereIs('upx')
        if upx:
            print("UPX detected, compressing target.")
            env.AddPostAction(target, Action('upx -qqq --best $TARGET'))
        else:
            print("UPX not found, skipping compression.")
    else:
        print("UPX compression is disabled")

def setFastRamFlags(env, target):
    env.AddPostAction(target, Action(env["MINTFLAGS"] + ' --mfastram --mfastload --mfastalloc $TARGET'))

def getVersion(env):
    git = env.WhereIs('git')
    if git:
        import subprocess
		# get current branch name
        p = subprocess.Popen('git rev-parse --abbrev-ref HEAD', shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        branchName = p.stdout.readline().rstrip().decode("utf-8")
        # get revision list
        gitRevisionListCmd = 'git rev-list --count ' + branchName
        p = subprocess.Popen(gitRevisionListCmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        return p.stdout.readline().rstrip().decode("utf-8")
    else:
        print("git not found")

# Check if we're in the source dir and abort
builddir = os.path.normpath(os.path.abspath(GetLaunchDir()))
srcdir = os.path.normpath(os.path.abspath("."))
if builddir == srcdir:
    print("Error: You can't build in the source directory!")
    Exit(1)

# Move scons database to builddir to avoid pollution
SConsignFile(os.path.join(builddir, '.sconsign.dblite'))

hostEnv = Environment(ENV = {'PATH' : os.environ['PATH']})
targetEnv = setupToolchain(hostEnv.Clone())

# Optionally use libcmini
detectLibCMini(targetEnv)

targetEnv['VERSION'] = getVersion(hostEnv)
targetEnv.Append(CPPDEFINES={'VERSION' : getVersion(hostEnv)})
targetEnv.Append(CPPDEFINES={'DEBUG' : 0})
targetEnv.Append(CPPDEFINES={'DUIP_CONF_BYTE_ORDER' : "BIG_ENDIAN"})

print("Building in: " + builddir)

def buildDriverVariant(hostEnv, targetEnv, varian_name):
    target = hostEnv.SConscript(
        "src/SConscript",
        duplicate = 0,
        exports=['hostEnv', 'targetEnv', 'varian_name'],
        variant_dir = builddir + '/' + varian_name,
        src_dir = "../" )
    # Optionally compress the binary with UPX
    compressProgramMaybe(targetEnv, target)
    # Load into TT ram if possible
    setFastRamFlags(targetEnv, target)
    return target

# Netusbee binary
netusbeeTargetEnv = targetEnv.Clone()
targetNetusbee = buildDriverVariant(hostEnv, netusbeeTargetEnv, "netusbee")

# USB ASIX binary
usbTartegEnv = targetEnv.Clone()
usbTartegEnv.Append(CPPDEFINES={'USB_DRIVER':1})
usbTartegEnv.Append(CPPDEFINES={'USB_PRINTSTATUS':1})
targetUSB = buildDriverVariant(hostEnv, usbTartegEnv, "usb")

# SLIP binary
slipTartegEnv = targetEnv.Clone()
slipTartegEnv.Append(CPPDEFINES={'SLIP_DRIVER':1})
slipTartegEnv.Append(CPPDEFINES={'WITHOUT_DHCP':1})
targetSLIP = buildDriverVariant(hostEnv, slipTartegEnv, "slip")

num_cpu = int(os.environ.get('NUMBER_OF_PROCESSORS', 2))
SetOption('num_jobs', num_cpu)
print("running with %d jobs." % GetOption('num_jobs')) 

targets = [targetNetusbee, targetUSB, targetSLIP]

Default(targets)
