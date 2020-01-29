################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../hal/CortexM3/GNU/hal.S \
../hal/CortexM3/GNU/hw_reg_access.S 

OBJS += \
./hal/CortexM3/GNU/hal.o \
./hal/CortexM3/GNU/hw_reg_access.o 

S_UPPER_DEPS += \
./hal/CortexM3/GNU/hal.d \
./hal/CortexM3/GNU/hw_reg_access.d 


# Each subdirectory must supply rules for building sources it contributes
hal/CortexM3/GNU/%.o: ../hal/CortexM3/GNU/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\Header_C_files" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\CMSIS" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\CMSIS\startup_gcc" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreI2C" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreSPI" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\CoreUARTapb" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_gpio" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_hpdma" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_i2c" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_nvm" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_pdma" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_spi" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_sys_services" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_timer" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers\mss_uart" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers_config" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\drivers_config\sys_config" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal\CortexM3" -I"C:\Users\InspireSat4\Documents\IS1\IS1_hw_redesign\is1_hw_redesign_firmware_v2\IS1_FSW_wksps\test\hal\CortexM3\GNU" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


