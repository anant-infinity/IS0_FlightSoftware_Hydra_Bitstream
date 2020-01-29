################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Header_C_files/hal/CortexM3/GNU/hal.S \
../Header_C_files/hal/CortexM3/GNU/hw_reg_access.S 

OBJS += \
./Header_C_files/hal/CortexM3/GNU/hal.o \
./Header_C_files/hal/CortexM3/GNU/hw_reg_access.o 

S_UPPER_DEPS += \
./Header_C_files/hal/CortexM3/GNU/hal.d \
./Header_C_files/hal/CortexM3/GNU/hw_reg_access.d 


# Each subdirectory must supply rules for building sources it contributes
Header_C_files/hal/CortexM3/GNU/%.o: ../Header_C_files/hal/CortexM3/GNU/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\Header_C_files" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\CMSIS" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\CMSIS\startup_gcc" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\CoreI2C" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\CoreSPI" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\CoreUARTapb" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_gpio" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_hpdma" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_i2c" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_nvm" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_pdma" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_spi" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_sys_services" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_timer" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers\mss_uart" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers_config" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\drivers_config\sys_config" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\hal" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\hal\CortexM3" -I"C:\Users\chandranadmin\Documents\IS1_FSW_wksps\test\hal\CortexM3\GNU" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


