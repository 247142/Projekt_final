################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/misc/stm32_mem.c \
../Utilities/misc/stm32_systime.c \
../Utilities/misc/stm32_tiny_sscanf.c \
../Utilities/misc/stm32_tiny_vsnprintf.c 

OBJS += \
./Utilities/misc/stm32_mem.o \
./Utilities/misc/stm32_systime.o \
./Utilities/misc/stm32_tiny_sscanf.o \
./Utilities/misc/stm32_tiny_vsnprintf.o 

C_DEPS += \
./Utilities/misc/stm32_mem.d \
./Utilities/misc/stm32_systime.d \
./Utilities/misc/stm32_tiny_sscanf.d \
./Utilities/misc/stm32_tiny_vsnprintf.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/misc/%.o Utilities/misc/%.su Utilities/misc/%.cyclo: ../Utilities/misc/%.c Utilities/misc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DPHY_40nm_3_00_a -DMAC -DMAC_LAYER -DDISABLE_RFTS_EXT_EVNT_HNDLR=1 -DUSE_HAL_DRIVER -DSTM32WBA55xx -c -I../Core/Inc -I../System/Interfaces -I../System/Config -I../System/Config/Log -I../System/Config/LowPower -I../System/Config/Debug_GPIO -I../STM32_WPAN/Target -I../STM32_WPAN/App -I../Utilities/trace/adv_trace -I../Drivers/STM32WBAxx_HAL_Driver/Inc -I../Drivers/STM32WBAxx_HAL_Driver/Inc/Legacy -I../Projects/Common/WPAN/Interfaces -I../Projects/Common/WPAN/Modules -I../Projects/Common/WPAN/Modules/BasicAES -I../Projects/Common/WPAN/Modules/RTDebug -I../Projects/Common/WPAN/Modules/SerialCmdInterpreter -I../Projects/Common/WPAN/Modules/Log -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/tim_serv -I../Utilities/lpm/tiny_lpm -I../Middlewares/ST/STM32_WPAN -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/config/ieee_15_4_basic -I../Drivers/CMSIS/Device/ST/STM32WBAxx/Include -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc/_40nm_reg_files -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc/ot_inc -I../Middlewares/ST/STM32_WPAN/link_layer/ll_sys/inc -I../Middlewares/ST/STM32_WPAN/mac_802_15_4/core/inc -I../Middlewares/ST/STM32_WPAN/zigbee/include -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/general -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/key -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/se -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/security -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/zd -I../Middlewares/ST/STM32_WPAN/zigbee/stack/port/mac -I../Middlewares/ST/STM32_WPAN/zigbee/stack/port/stm32wba -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Utilities-2f-misc

clean-Utilities-2f-misc:
	-$(RM) ./Utilities/misc/stm32_mem.cyclo ./Utilities/misc/stm32_mem.d ./Utilities/misc/stm32_mem.o ./Utilities/misc/stm32_mem.su ./Utilities/misc/stm32_systime.cyclo ./Utilities/misc/stm32_systime.d ./Utilities/misc/stm32_systime.o ./Utilities/misc/stm32_systime.su ./Utilities/misc/stm32_tiny_sscanf.cyclo ./Utilities/misc/stm32_tiny_sscanf.d ./Utilities/misc/stm32_tiny_sscanf.o ./Utilities/misc/stm32_tiny_sscanf.su ./Utilities/misc/stm32_tiny_vsnprintf.cyclo ./Utilities/misc/stm32_tiny_vsnprintf.d ./Utilities/misc/stm32_tiny_vsnprintf.o ./Utilities/misc/stm32_tiny_vsnprintf.su

.PHONY: clean-Utilities-2f-misc

