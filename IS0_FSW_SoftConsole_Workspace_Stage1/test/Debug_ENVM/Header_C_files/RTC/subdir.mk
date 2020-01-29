################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Header_C_files/RTC/rtc.c 

OBJS += \
./Header_C_files/RTC/rtc.o 

C_DEPS += \
./Header_C_files/RTC/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
Header_C_files/RTC/%.o: ../Header_C_files/RTC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\Header_C_files" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS\startup_gcc" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreI2C" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreSPI" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\CoreUARTapb" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_gpio" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_hpdma" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_i2c" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_nvm" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_pdma" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_sys_services" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_timer" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers\mss_uart" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers_config" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\drivers_config\sys_config" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal\CortexM3" -I"C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\hal\CortexM3\GNU" -std=gnu11 -O0 --specs=cmsis.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


