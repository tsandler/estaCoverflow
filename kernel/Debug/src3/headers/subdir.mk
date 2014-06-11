################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src3/headers/manejoDelPCB.c 

OBJS += \
./src3/headers/manejoDelPCB.o 

C_DEPS += \
./src3/headers/manejoDelPCB.d 


# Each subdirectory must supply rules for building sources it contributes
src3/headers/%.o: ../src3/headers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2014-1c-out-of-range/commons" -O0 -g3 -Wall -lrt -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


