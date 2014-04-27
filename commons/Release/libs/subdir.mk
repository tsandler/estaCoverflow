################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/manejoDelPCB.c \
../libs/manejoStack.c \
../libs/primitivas.c \
../libs/sockets.c 

OBJS += \
./libs/manejoDelPCB.o \
./libs/manejoStack.o \
./libs/primitivas.o \
./libs/sockets.o 

C_DEPS += \
./libs/manejoDelPCB.d \
./libs/manejoStack.d \
./libs/primitivas.d \
./libs/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
libs/%.o: ../libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


