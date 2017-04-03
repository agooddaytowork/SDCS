################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./GUI.obj" \
"./SDCS_beta.obj" \
"./TimeOut.obj" \
"./msp432_startup_ccs.obj" \
"./printf.obj" \
"./wrappedI2C/wrappedi2c.obj" \
"./RFID/RC522-multi.obj" \
"./OLEDlibrary/OLED.obj" \
"./GaugeSensor/GaugeSensor.obj" \
"./GaugeSensor/RingBuffer.obj" \
"./CAN/EDCTUComm.obj" \
"./CAN/mcp2515.obj" \
"../msp432p401r.cmd" \
"../msp432p4xx_driverlib.lib" \
$(GEN_CMDS__FLAG) \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include wrappedI2C/subdir_vars.mk
-include RFID/subdir_vars.mk
-include OLEDlibrary/subdir_vars.mk
-include GaugeSensor/subdir_vars.mk
-include CAN/subdir_vars.mk
-include subdir_rules.mk
-include wrappedI2C/subdir_rules.mk
-include RFID/subdir_rules.mk
-include OLEDlibrary/subdir_rules.mk
-include GaugeSensor/subdir_rules.mk
-include CAN/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SDCS_beta.out \

EXE_OUTPUTS__QUOTED += \
"SDCS_beta.out" \

BIN_OUTPUTS += \
SDCS_beta.hex \

BIN_OUTPUTS__QUOTED += \
"SDCS_beta.hex" \


# All Target
all: SDCS_beta.out

# Tool invocations
SDCS_beta.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP432 Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --opt_for_speed=0 --advice:power=all -g --gcc --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs --diag_wrap=off --diag_warning=225 --display_error_number -z -m"SDCS_beta.map" --heap_size=1024 --stack_size=512 -i"C:/ti/ccsv6/ccs_base/arm/include" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --display_error_number --diag_wrap=off --warn_sections --xml_link_info="SDCS_beta_linkInfo.xml" --rom_model -o "SDCS_beta.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

SDCS_beta.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: MSP432 Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "SDCS_beta.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "GUI.pp" "SDCS_beta.pp" "TimeOut.pp" "msp432_startup_ccs.pp" "printf.pp" "wrappedI2C\wrappedi2c.pp" "RFID\RC522-multi.pp" "OLEDlibrary\OLED.pp" "GaugeSensor\GaugeSensor.pp" "GaugeSensor\RingBuffer.pp" "CAN\EDCTUComm.pp" "CAN\mcp2515.pp" 
	-$(RM) "GUI.obj" "SDCS_beta.obj" "TimeOut.obj" "msp432_startup_ccs.obj" "printf.obj" "wrappedI2C\wrappedi2c.obj" "RFID\RC522-multi.obj" "OLEDlibrary\OLED.obj" "GaugeSensor\GaugeSensor.obj" "GaugeSensor\RingBuffer.obj" "CAN\EDCTUComm.obj" "CAN\mcp2515.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

