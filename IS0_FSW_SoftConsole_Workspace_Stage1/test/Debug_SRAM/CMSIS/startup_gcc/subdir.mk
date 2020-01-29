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
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\Header_C_files" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\CMSIS" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\CMSIS\startup_gcc" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreI2C" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreSPI" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreUARTapb" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_gpio" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_hpdma" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_i2c" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_nvm" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_pdma" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_spi" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_sys_services" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_timer" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_uart" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers_config" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers_config\sys_config" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal\CortexM3" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal\CortexM3\GNU" -std=gnu11 -O0 -v -specs=cmsis.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\Header_C_files" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\CMSIS" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\CMSIS\startup_gcc" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreI2C" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreSPI" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreUARTapb" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_gpio" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_hpdma" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_i2c" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_nvm" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_pdma" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_spi" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_sys_services" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_timer" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_uart" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers_config" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers_config\sys_config" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal\CortexM3" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal\CortexM3\GNU" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


