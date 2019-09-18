import os

# !! Update these aths to point to your respective directories !!
GNU_PATH = '/home/gray/embedded/gcc-arm-none-eabi/bin/'

# Change the compiled name of the file below
# ex. compileTarget = 'exampleProject'
compileTarget = 'main'

# Create Communal build directory to store all the .o's
VariantDir('build/board', 'board')
VariantDir('build/CMSIS', 'CMSIS')
VariantDir('build/drivers', 'drivers')
VariantDir('build/lib', 'lib')
VariantDir('build/System', 'System')
VariantDir('build/utilities', 'utilities')
VariantDir('build/source', 'source')

env = Environment(ENV = os.environ)

env['AR'] = GNU_PATH+'arm-none-eabi-ar'
env['AS'] = GNU_PATH+'arm-none-eabi-gcc'
env['ASCOM'] = '$AS $ASFLAGS $_CPPINCFLAGS -o $TARGET $SOURCES'
env['CC'] = GNU_PATH+'arm-none-eabi-gcc'
env['CXX'] = GNU_PATH+'arm-none-eabi-g++'
env['CXXCOM'] = '$CXX -o $TARGET -c $CXXFLAGS $_CCCOMCOM $SOURCES'
env['LINK'] = GNU_PATH+'arm-none-eabi-g++'                
env['RANLIB'] = GNU_PATH+'arm-none-eabi-ranlib'
env['OBJCOPY'] = GNU_PATH+'arm-none-eabi-objcopy'
env['PROGSUFFIX'] = '.elf'
OBJDUMP = GNU_PATH+'arm-none-eabi-objdump'
READELF = GNU_PATH+'arm-none-eabi-readelf'


env['ASFLAGS'] = '-g -DDEBUG -D__STARTUP_CLEAR_BSS \
        -D__STARTUP_INITIALIZE_NONCACHEDATA -Wall -fno-common \
        -ffunction-sections -fdata-sections -ffreestanding \
        -fno-builtin -mthumb -mapcs -std=gnu99 -mcpu=cortex-m4 \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16'

env['CCFLAGS'] = '-O0 -g -DDEBUG -DCPU_MKE18F512VLH16 \
        -DTWR_KE18F -DTOWER -Wall -fno-common \
        -ffunction-sections -fdata-sections -ffreestanding \
        -fno-builtin -mthumb -mapcs -std=gnu99 -mcpu=cortex-m4 \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP'

env['CXXFLAGS'] = '-O0 -g -DDEBUG -Wall \
        -fno-common -ffunction-sections -fdata-sections \
        -ffreestanding -fno-builtin -mthumb -mapcs \
        -fno-rtti -fno-exceptions -mcpu=cortex-m4 \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP \
        -DCPU_MKE18F512VLH16 '

env.Append(CPPPATH = [
    'board',
    'CMSIS',
    'drivers',
    'utilities',
    'lib',
    'source',
    'System',
    '.'
])

env['LINKFLAGS'] = '-O0 -g -DDEBUG -Wall \
    -fno-common -ffunction-sections -fdata-sections \
    -ffreestanding -fno-builtin -mthumb -mapcs \
    -fno-rtti -fno-exceptions -mcpu=cortex-m4 \
    -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP \
    -DCPU_MKE18F512VLH16 -specs=nano.specs \
    -specs=nosys.specs -Wall -fno-common \
    -ffunction-sections -fdata-sections \
    -ffreestanding -fno-builtin -mthumb \
    -mapcs -Xlinker --gc-sections -Xlinker \
    -static -Xlinker -z -Xlinker muldefs \
    -mcpu=cortex-m4 -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 -TMKE18F512xxx16_flash.ld -static'

cppsource = Glob('build/lib/*.cpp') +\
    Glob('build/System/*.cpp')

csource = Glob('build/board/*.c') +\
    Glob('build/CMSIS/*.c') +\
    Glob('build/drivers/*.c') +\
    Glob('build/utilities/*.c')

asm = Glob('build/CMSIS/*.S')

# Make Static BSP Library
env.StaticLibrary(target='bsp', source=asm+cppsource+csource)

# Run the compile command and create .elf
# !! This is not necessary in the BSP !!
#env.Program(compileTarget, source=Glob('build/source/*.cpp'), LIBS=['bsp'], LIBPATH=['.'])

# Create .lst from .elf
# !! This is not necessary in the BSP !!
#env.Command(compileTarget+".lst", compileTarget+".elf", \
#    OBJDUMP+" -D " + compileTarget+".elf" + " > " + compileTarget+".lst")

# Print Memory Map -> .elf Headers
# !! This is not necessary in the BSP !!
#env.Command(compileTarget, compileTarget+".elf", READELF+" -e " + compileTarget+".elf")
