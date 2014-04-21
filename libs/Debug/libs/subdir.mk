################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/manejoDelPCB.c \
../libs/manejoStack.c \
../libs/primitivas.c 

OBJS += \
./libs/manejoDelPCB.o \
./libs/manejoStack.o \
./libs/primitivas.o 

C_DEPS += \
./libs/manejoDelPCB.d \
./libs/manejoStack.d \
./libs/primitivas.d 


# Each subdirectory must supply rules for building sources it contributes
libs/%.o: ../libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


