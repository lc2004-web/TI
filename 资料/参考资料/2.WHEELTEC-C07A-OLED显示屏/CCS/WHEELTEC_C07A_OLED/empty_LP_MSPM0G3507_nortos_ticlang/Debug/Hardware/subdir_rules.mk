################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Hardware/%.o: ../Hardware/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"E:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"E:/TI/TI/资料/参考资料/2.WHEELTEC-C07A-OLED显示屏/CCS/WHEELTEC_C07A_OLED/empty_LP_MSPM0G3507_nortos_ticlang/BSP/MPU6050" -I"E:/TI/TI/资料/参考资料/2.WHEELTEC-C07A-OLED显示屏/CCS/WHEELTEC_C07A_OLED/empty_LP_MSPM0G3507_nortos_ticlang/BSP/MPU6050/DMP" -I"E:/TI/TI/资料/参考资料/2.WHEELTEC-C07A-OLED显示屏/CCS/WHEELTEC_C07A_OLED/empty_LP_MSPM0G3507_nortos_ticlang/CONTROL" -I"E:/TI/TI/资料/参考资料/2.WHEELTEC-C07A-OLED显示屏/CCS/WHEELTEC_C07A_OLED/empty_LP_MSPM0G3507_nortos_ticlang/HARDWARE" -I"E:/TI/TI/资料/参考资料/2.WHEELTEC-C07A-OLED显示屏/CCS/WHEELTEC_C07A_OLED/empty_LP_MSPM0G3507_nortos_ticlang" -I"E:/TI/TI/资料/参考资料/2.WHEELTEC-C07A-OLED显示屏/CCS/WHEELTEC_C07A_OLED/empty_LP_MSPM0G3507_nortos_ticlang/Debug" -I"E:/TI/CCS/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"E:/TI/CCS/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -MMD -MP -MF"Hardware/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


