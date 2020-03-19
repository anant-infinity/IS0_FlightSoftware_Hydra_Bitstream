################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include hal/CortexM3/GNU/subdir.mk
-include hal/CortexM3/subdir.mk
-include drivers_config/sys_config/subdir.mk
-include drivers_config/mss_spi/subdir.mk
-include drivers/mss_uart/subdir.mk
-include drivers/mss_sys_services/subdir.mk
-include drivers/mss_pdma/subdir.mk
-include drivers/mss_nvm/subdir.mk
-include drivers/mss_i2c/subdir.mk
-include drivers/mss_hpdma/subdir.mk
-include drivers/mss_gpio/subdir.mk
-include drivers/CoreUARTapb/subdir.mk
-include drivers/CoreSPI/subdir.mk
-include drivers/CoreI2C/subdir.mk
-include Header_C_files/Utils/subdir.mk
-include Header_C_files/Seq_ops/subdir.mk
-include Header_C_files/SPI_Flash/subdir.mk
-include Header_C_files/SENSOR_BOARD/subdir.mk
-include Header_C_files/SD_CARD/subdir.mk
-include Header_C_files/RTC/subdir.mk
-include Header_C_files/PSLV_UART/subdir.mk
-include Header_C_files/I2C/subdir.mk
-include Header_C_files/GPIOs/subdir.mk
-include Header_C_files/EPS/subdir.mk
-include Header_C_files/CCSDS/subdir.mk
-include CMSIS/startup_gcc/subdir.mk
-include CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
test.hex \

SECONDARY_SIZE += \
test.siz \


# All Target
all: test.elf secondary-outputs

# Tool invocations
test.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "C:\Users\Anant\Documents\IS0_FSW_Hydra_Bitstream_Files\IS0_FlightSoftware_Hydra_Bitstream\IS0_FSW_SoftConsole_Workspace_Stage1\test\CMSIS\startup_gcc\production-smartfusion2-execute-in-place.ld" -Xlinker --gc-sections -Wl,-Map,"test.map" --specs=nano.specs -o "test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

test.hex: test.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "test.elf"  "test.hex"
	@echo 'Finished building: $@'
	@echo ' '

test.siz: test.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=sysv "test.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) test.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
