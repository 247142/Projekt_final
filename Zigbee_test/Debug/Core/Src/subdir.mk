################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/DEV_Config.c \
../Core/Src/EPD_1in9.c \
../Core/Src/app_entry.c \
../Core/Src/epd_custom.c \
../Core/Src/main.c \
../Core/Src/stm32wbaxx_hal_msp.c \
../Core/Src/stm32wbaxx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32wbaxx.c 

OBJS += \
./Core/Src/DEV_Config.o \
./Core/Src/EPD_1in9.o \
./Core/Src/app_entry.o \
./Core/Src/epd_custom.o \
./Core/Src/main.o \
./Core/Src/stm32wbaxx_hal_msp.o \
./Core/Src/stm32wbaxx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32wbaxx.o 

C_DEPS += \
./Core/Src/DEV_Config.d \
./Core/Src/EPD_1in9.d \
./Core/Src/app_entry.d \
./Core/Src/epd_custom.d \
./Core/Src/main.d \
./Core/Src/stm32wbaxx_hal_msp.d \
./Core/Src/stm32wbaxx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32wbaxx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DPHY_40nm_3_00_a -DMAC -DMAC_LAYER -DDISABLE_RFTS_EXT_EVNT_HNDLR=1 -DUSE_HAL_DRIVER -DSTM32WBA55xx -c -I../Core/Inc -I../System/Interfaces -I../System/Config -I../System/Config/Log -I../System/Config/LowPower -I../System/Config/Debug_GPIO -I../STM32_WPAN/Target -I../STM32_WPAN/App -I../Utilities/trace/adv_trace -I../Drivers/STM32WBAxx_HAL_Driver/Inc -I../Drivers/STM32WBAxx_HAL_Driver/Inc/Legacy -I../Projects/Common/WPAN/Interfaces -I../Projects/Common/WPAN/Modules -I../Projects/Common/WPAN/Modules/BasicAES -I../Projects/Common/WPAN/Modules/RTDebug -I../Projects/Common/WPAN/Modules/SerialCmdInterpreter -I../Projects/Common/WPAN/Modules/Log -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/tim_serv -I../Utilities/lpm/tiny_lpm -I../Middlewares/ST/STM32_WPAN -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/config/ieee_15_4_basic -I../Drivers/CMSIS/Device/ST/STM32WBAxx/Include -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc/_40nm_reg_files -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc/ot_inc -I../Middlewares/ST/STM32_WPAN/link_layer/ll_sys/inc -I../Middlewares/ST/STM32_WPAN/mac_802_15_4/core/inc -I../Middlewares/ST/STM32_WPAN/zigbee/include -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/general -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/key -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/se -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/security -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/zd -I../Middlewares/ST/STM32_WPAN/zigbee/stack/port/mac -I../Middlewares/ST/STM32_WPAN/zigbee/stack/port/stm32wba -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/DEV_Config.cyclo ./Core/Src/DEV_Config.d ./Core/Src/DEV_Config.o ./Core/Src/DEV_Config.su ./Core/Src/EPD_1in9.cyclo ./Core/Src/EPD_1in9.d ./Core/Src/EPD_1in9.o ./Core/Src/EPD_1in9.su ./Core/Src/app_entry.cyclo ./Core/Src/app_entry.d ./Core/Src/app_entry.o ./Core/Src/app_entry.su ./Core/Src/epd_custom.cyclo ./Core/Src/epd_custom.d ./Core/Src/epd_custom.o ./Core/Src/epd_custom.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32wbaxx_hal_msp.cyclo ./Core/Src/stm32wbaxx_hal_msp.d ./Core/Src/stm32wbaxx_hal_msp.o ./Core/Src/stm32wbaxx_hal_msp.su ./Core/Src/stm32wbaxx_it.cyclo ./Core/Src/stm32wbaxx_it.d ./Core/Src/stm32wbaxx_it.o ./Core/Src/stm32wbaxx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32wbaxx.cyclo ./Core/Src/system_stm32wbaxx.d ./Core/Src/system_stm32wbaxx.o ./Core/Src/system_stm32wbaxx.su

.PHONY: clean-Core-2f-Src

