################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
targetConfigs/System/%.o: ../targetConfigs/System/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/Lenovo/Desktop/CCSProject/MSPM0G3507/16My_Mpu6050" -I"C:/Users/Lenovo/Desktop/CCSProject/MSPM0G3507/16My_Mpu6050/Debug" -I"D:/TI/CCS/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"D:/TI/CCS/mspm0_sdk_2_10_00_04/source" -I"C:/Users/Lenovo/Desktop/CCSProject/MSPM0G3507/06_PWM/targetConfigs/System" -I"C:/Users/Lenovo/Desktop/CCSProject/MSPM0G3507/06_PWM/targetConfigs/Hardware" -gdwarf-3 -Wall -MMD -MP -MF"targetConfigs/System/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


