################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/src/headers/variablesCompartidas.c 

OBJS += \
./kernel/src/headers/variablesCompartidas.o 

C_DEPS += \
./kernel/src/headers/variablesCompartidas.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/src/headers/%.o: ../kernel/src/headers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


