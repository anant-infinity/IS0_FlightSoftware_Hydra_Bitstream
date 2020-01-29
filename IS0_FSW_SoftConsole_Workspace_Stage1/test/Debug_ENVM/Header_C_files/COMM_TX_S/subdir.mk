################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Header_C_files/COMM_TX_S/sband.c 

OBJS += \
./Header_C_files/COMM_TX_S/sband.o 

C_DEPS += \
./Header_C_files/COMM_TX_S/sband.d 


# Each subdirectory must supply rules for building sources it contributes
Header_C_files/COMM_TX_S/%.o: ../Header_C_files/COMM_TX_S/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\Header_C_files" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\CMSIS" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\CMSIS\startup_gcc" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\CoreI2C" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\CoreSPI" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\CoreUARTapb" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_gpio" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_hpdma" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_i2c" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_nvm" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_pdma" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_sys_services" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_timer" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers\mss_uart" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers_config" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\drivers_config\sys_config" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\hal" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\hal\CortexM3" -I"D:\Final_Year_Project_Work\IS-0-P4\Software_Wksps\is1_hw_redesign_firmware_v5\IS1_FSW_wksps\test\hal\CortexM3\GNU" -std=gnu11 -O0 --specs=cmsis.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


