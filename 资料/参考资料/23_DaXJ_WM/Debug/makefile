################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./Hardware/No_Mcu_Ganv_Grayscale_Sensor.o" \
"./Hardware/adc.o" \
"./Hardware/bsp_tb6612.o" \
"./Hardware/delay.o" \
"./Hardware/flash.o" \
"./Hardware/key.o" \
"./Hardware/led.o" \
"./Hardware/OLED/board.o" \
"./Hardware/OLED/oled.o" \
"./Hardware/src/bsp_mpu6050.o" \
"./Hardware/src/inv_mpu.o" \
"./Hardware/src/inv_mpu_dmp_motion_driver.o" \
"./User/PID.o" \
"./User/main.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Hardware/subdir_vars.mk
-include Hardware/OLED/subdir_vars.mk
-include Hardware/src/subdir_vars.mk
-include User/subdir_vars.mk
-include subdir_rules.mk
-include Hardware/subdir_rules.mk
-include Hardware/OLED/subdir_rules.mk
-include Hardware/src/subdir_rules.mk
-include User/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
23_DaXJ_WM.out 

EXE_OUTPUTS__QUOTED += \
"23_DaXJ_WM.out" 


# All Target
all: $(OBJS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "23_DaXJ_WM.out"

# Tool invocations
23_DaXJ_WM.out: $(OBJS) $(GEN_CMDS)
	@echo 'Arm Linker - building target: "$@"'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -gdwarf-3 -Wall -Wl,-m"23_DaXJ_WM.map" -Wl,-i"D:/TI/CCS/mspm0_sdk_2_10_00_04/source" -Wl,-i"C:/Users/Lenovo/Desktop/CCSProject/MSPM0G3507/23_DaXJ_WM" -Wl,-i"C:/Users/Lenovo/Desktop/CCSProject/MSPM0G3507/23_DaXJ_WM/Debug/syscfg" -Wl,-i"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="23_DaXJ_WM_linkInfo.xml" -Wl,--rom_model -o "23_DaXJ_WM.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "Hardware\No_Mcu_Ganv_Grayscale_Sensor.o" "Hardware\adc.o" "Hardware\bsp_tb6612.o" "Hardware\delay.o" "Hardware\flash.o" "Hardware\key.o" "Hardware\led.o" "Hardware\OLED\board.o" "Hardware\OLED\oled.o" "Hardware\src\bsp_mpu6050.o" "Hardware\src\inv_mpu.o" "Hardware\src\inv_mpu_dmp_motion_driver.o" "User\PID.o" "User\main.o" 
	-$(RM) "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "Hardware\No_Mcu_Ganv_Grayscale_Sensor.d" "Hardware\adc.d" "Hardware\bsp_tb6612.d" "Hardware\delay.d" "Hardware\flash.d" "Hardware\key.d" "Hardware\led.d" "Hardware\OLED\board.d" "Hardware\OLED\oled.d" "Hardware\src\bsp_mpu6050.d" "Hardware\src\inv_mpu.d" "Hardware\src\inv_mpu_dmp_motion_driver.d" "User\PID.d" "User\main.d" 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

