################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/headers/manejoStack.c \
../src/headers/primitivas.c 

OBJS += \
./src/headers/manejoStack.o \
./src/headers/primitivas.o 

C_DEPS += \
./src/headers/manejoStack.d \
./src/headers/primitivas.d 


# Each subdirectory must supply rules for building sources it contributes
src/headers/%.o: ../src/headers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2014-1c-out-of-range/commons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


