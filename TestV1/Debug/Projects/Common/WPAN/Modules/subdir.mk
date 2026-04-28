################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Projects/Common/WPAN/Modules/app_sys.c \
../Projects/Common/WPAN/Modules/otp.c \
../Projects/Common/WPAN/Modules/scm.c \
../Projects/Common/WPAN/Modules/stm_list.c \
../Projects/Common/WPAN/Modules/stm_queue.c 

OBJS += \
./Projects/Common/WPAN/Modules/app_sys.o \
./Projects/Common/WPAN/Modules/otp.o \
./Projects/Common/WPAN/Modules/scm.o \
./Projects/Common/WPAN/Modules/stm_list.o \
./Projects/Common/WPAN/Modules/stm_queue.o 

C_DEPS += \
./Projects/Common/WPAN/Modules/app_sys.d \
./Projects/Common/WPAN/Modules/otp.d \
./Projects/Common/WPAN/Modules/scm.d \
./Projects/Common/WPAN/Modules/stm_list.d \
./Projects/Common/WPAN/Modules/stm_queue.d 


# Each subdirectory must supply rules for building sources it contributes
Projects/Common/WPAN/Modules/%.o Projects/Common/WPAN/Modules/%.su Projects/Common/WPAN/Modules/%.cyclo: ../Projects/Common/WPAN/Modules/%.c Projects/Common/WPAN/Modules/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DPHY_40nm_3_00_a -DMAC -DMAC_LAYER -DDISABLE_RFTS_EXT_EVNT_HNDLR=1 -DUSE_HAL_DRIVER -DSTM32WBA55xx -c -I../Core/Inc -I../System/Interfaces -I../System/Config -I../System/Config/Log -I../System/Config/LowPower -I../System/Config/Debug_GPIO -I../STM32_WPAN/Target -I../STM32_WPAN/App -I../Utilities/trace/adv_trace -I../Drivers/STM32WBAxx_HAL_Driver/Inc -I../Drivers/STM32WBAxx_HAL_Driver/Inc/Legacy -I../Projects/Common/WPAN/Interfaces -I../Projects/Common/WPAN/Modules -I../Projects/Common/WPAN/Modules/BasicAES -I../Projects/Common/WPAN/Modules/RTDebug -I../Projects/Common/WPAN/Modules/SerialCmdInterpreter -I../Projects/Common/WPAN/Modules/Log -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/tim_serv -I../Utilities/lpm/tiny_lpm -I../Middlewares/ST/STM32_WPAN -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/config/ieee_15_4_basic -I../Drivers/CMSIS/Device/ST/STM32WBAxx/Include -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc/_40nm_reg_files -I../Middlewares/ST/STM32_WPAN/link_layer/ll_cmd_lib/inc/ot_inc -I../Middlewares/ST/STM32_WPAN/link_layer/ll_sys/inc -I../Middlewares/ST/STM32_WPAN/mac_802_15_4/core/inc -I../Middlewares/ST/STM32_WPAN/zigbee/include -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/general -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/key -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/se -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/security -I../Middlewares/ST/STM32_WPAN/zigbee/include/zcl/zd -I../Middlewares/ST/STM32_WPAN/zigbee/stack/port/mac -I../Middlewares/ST/STM32_WPAN/zigbee/stack/port/stm32wba -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Projects-2f-Common-2f-WPAN-2f-Modules

clean-Projects-2f-Common-2f-WPAN-2f-Modules:
	-$(RM) ./Projects/Common/WPAN/Modules/app_sys.cyclo ./Projects/Common/WPAN/Modules/app_sys.d ./Projects/Common/WPAN/Modules/app_sys.o ./Projects/Common/WPAN/Modules/app_sys.su ./Projects/Common/WPAN/Modules/otp.cyclo ./Projects/Common/WPAN/Modules/otp.d ./Projects/Common/WPAN/Modules/otp.o ./Projects/Common/WPAN/Modules/otp.su ./Projects/Common/WPAN/Modules/scm.cyclo ./Projects/Common/WPAN/Modules/scm.d ./Projects/Common/WPAN/Modules/scm.o ./Projects/Common/WPAN/Modules/scm.su ./Projects/Common/WPAN/Modules/stm_list.cyclo ./Projects/Common/WPAN/Modules/stm_list.d ./Projects/Common/WPAN/Modules/stm_list.o ./Projects/Common/WPAN/Modules/stm_list.su ./Projects/Common/WPAN/Modules/stm_queue.cyclo ./Projects/Common/WPAN/Modules/stm_queue.d ./Projects/Common/WPAN/Modules/stm_queue.o ./Projects/Common/WPAN/Modules/stm_queue.su

.PHONY: clean-Projects-2f-Common-2f-WPAN-2f-Modules

