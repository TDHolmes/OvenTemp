INCLUDE(CMakeForceCompiler)
# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

SET(CLICOLOR_FORCE ON)

# THE VERSION NUMBER
SET (MotorController_VERSION_MAJOR 0)
SET (MotorController_VERSION_MINOR 1)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)
SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)


# CURRENT DIRECTORY up a level
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR}/../)


# ARMGCC DIRECTORY, if null string will use environmental variables
SET(TOOLCHAIN_DIR "" CACHE STRING "Directory to use for ARM GCC")

# ------------------------------ FLAGS USED BY EVERYONE ------------------------------ #
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -flto")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mcpu=cortex-m0plus")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -Wall -Wdouble-promotion -Wextra")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mfloat-abi=soft")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mthumb")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fno-common")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -ffunction-sections")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fdata-sections")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -ffreestanding")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fno-builtin")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -std=gnu99")

# ------------------------------ ASSEMBLY LANGUAGE FLAGS - DEBUG ------------------------------ #
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -D__STARTUP_CLEAR_BSS")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ ASSEMBLY LANGUAGE FLAGS - DEBUG ------------------------------ #
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -D__STARTUP_CLEAR_BSS")
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -flto")
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ C LANGUAGE FLAGS - DEBUG ------------------------------ #
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCPU_MKL03Z32VFK4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DPRINTF_FLOAT_ENABLE=0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSCANF_FLOAT_ENABLE=0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DPRINTF_ADVANCED_ENABLE=0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSCANF_ADVANCED_ENABLE=0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DFRDM_KL03Z")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DFREEDOM")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MMD")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MP")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ C LANGUAGE FLAGS - RELEASE ------------------------------ #
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DCPU_MKL03Z32VFK4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DPRINTF_FLOAT_ENABLE=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSCANF_FLOAT_ENABLE=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DPRINTF_ADVANCED_ENABLE=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DSCANF_ADVANCED_ENABLE=0")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DFRDM_KL03Z")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DFREEDOM")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MMD")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MP")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ LINKER FLAGS - RELEASE ------------------------------ #
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} --specs=nano.specs")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker --gc-sections")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -static")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -z")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker muldefs")


# ------------------------------ LINKER FLAGS - DEBUG ------------------------------ #
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --specs=nano.specs")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker --gc-sections")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -static")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -z")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker muldefs")


# ------------------------------ INCLUDE DIRECTORIES ------------------------------ #

include_directories(${ProjDirPath})

include_directories(${ProjDirPath}MKL03Z4_drivers/CMSIS/Include)

include_directories(${ProjDirPath}MKL03Z4_drivers/devices)

include_directories(${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers)

include_directories(${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/utilities)

include_directories(${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4)


# ------------------------------ SOURCE FILES ------------------------------ #

add_executable(motor_driver.elf
	# user code
	"${ProjDirPath}motor_drivers.c"
	"${ProjDirPath}motor_drivers.h"
	"${ProjDirPath}motor_calc.c"
	"${ProjDirPath}motor_calc.h"
	"${ProjDirPath}constants.h"
	"${ProjDirPath}encoders.c"
	"${ProjDirPath}encoders.h"
	"${ProjDirPath}hardware.c"
	"${ProjDirPath}hardware.h"
	"${ProjDirPath}clock_config.c"
	"${ProjDirPath}clock_config.h"
	"${ProjDirPath}i2c_comms.c"
	"${ProjDirPath}i2c_comms.h"
	"${ProjDirPath}timer.c"
	"${ProjDirPath}timer.h"
	"${ProjDirPath}main.c"
	"${ProjDirPath}main.h"

	# NXP drivers and system files
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/gcc/startup_MKL03Z4.S"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_port.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_common.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_common.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/system_MKL03Z4.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/system_MKL03Z4.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_gpio.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_gpio.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_tpm.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_tpm.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_i2c.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_i2c.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_clock.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_clock.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_lptmr.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_lptmr.c"
	# UART stuff...
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_lpuart.h"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_lpuart.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/utilities/fsl_debug_console.c"
	"${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/utilities/fsl_debug_console.h"
	# Other stuff that was used before, but not anymore :p
	# "${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/pin_mux.h"
	# "${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/pin_mux.c"
	# "${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_smc.h"
	# "${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/drivers/fsl_smc.c"
)


# ------------------------------ LINKER FILES ------------------------------ #
set(LINKER_LOCATION "${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/gcc/")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${LINKER_LOCATION}MKL03Z32xxx4_flash.ld -static")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${LINKER_LOCATION}MKL03Z32xxx4_flash.ld -static")



# --------------------- RESOLVING CIRCULAR DEPENDENCIES -------------------- #
# http://stackoverflow.com/questions/5651869/gcc-what-are-the-start-group-and-end-group-command-line-options

TARGET_LINK_LIBRARIES(motor_driver.elf -Wl,--start-group)

target_link_libraries(motor_driver.elf debug m)

target_link_libraries(motor_driver.elf debug c)

target_link_libraries(motor_driver.elf debug gcc)

target_link_libraries(motor_driver.elf debug nosys)

target_link_libraries(motor_driver.elf optimized m)

target_link_libraries(motor_driver.elf optimized c)

target_link_libraries(motor_driver.elf optimized gcc)

target_link_libraries(motor_driver.elf optimized nosys)

TARGET_LINK_LIBRARIES(motor_driver.elf -Wl,--end-group)
