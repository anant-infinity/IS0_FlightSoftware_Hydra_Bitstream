################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/startup_gcc/newlib_stubs.c 

S_UPPER_SRCS += \
../CMSIS/startup_gcc/startup_m2sxxx.S 

OBJS += \
./CMSIS/startup_gcc/newlib_stubs.o \
./CMSIS/startup_gcc/startup_m2sxxx.o 

S_UPPER_DEPS += \
./CMSIS/startup_gcc/startup_m2sxxx.d 

C_DEPS += \
./CMSIS/startup_gcc/newlib_stubs.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\Header_C_files" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS\startup_gcc" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreI2C" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreSPI" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreUARTapb" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_gpio" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_hpdma" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_i2c" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_nvm" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_pdma" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_sys_services" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_timer" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_uart" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers_config" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers_config\sys_config" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal\CortexM3" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal\CortexM3\GNU" -std=gnu11 -O0 --specs=cmsis.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -x assembler-with-cpp -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\Header_C_files" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS\startup_gcc" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreI2C" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreSPI" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreUARTapb" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_gpio" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_hpdma" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_i2c" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_nvm" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_pdma" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_sys_services" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_timer" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_uart" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers_config" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers_config\sys_config" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal\CortexM3" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal\CortexM3\GNU" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


