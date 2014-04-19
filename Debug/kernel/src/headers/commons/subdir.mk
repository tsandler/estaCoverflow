################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/src/headers/commons/bitarray.c \
../kernel/src/headers/commons/config.c \
../kernel/src/headers/commons/error.c \
../kernel/src/headers/commons/log.c \
../kernel/src/headers/commons/process.c \
../kernel/src/headers/commons/string.c \
../kernel/src/headers/commons/temporal.c \
../kernel/src/headers/commons/txt.c 

OBJS += \
./kernel/src/headers/commons/bitarray.o \
./kernel/src/headers/commons/config.o \
./kernel/src/headers/commons/error.o \
./kernel/src/headers/commons/log.o \
./kernel/src/headers/commons/process.o \
./kernel/src/headers/commons/string.o \
./kernel/src/headers/commons/temporal.o \
./kernel/src/headers/commons/txt.o 

C_DEPS += \
./kernel/src/headers/commons/bitarray.d \
./kernel/src/headers/commons/config.d \
./kernel/src/headers/commons/error.d \
./kernel/src/headers/commons/log.d \
./kernel/src/headers/commons/process.d \
./kernel/src/headers/commons/string.d \
./kernel/src/headers/commons/temporal.d \
./kernel/src/headers/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/src/headers/commons/%.o: ../kernel/src/headers/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


