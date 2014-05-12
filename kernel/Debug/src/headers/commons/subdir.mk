################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/headers/commons/bitarray.c \
../src/headers/commons/config.c \
../src/headers/commons/error.c \
../src/headers/commons/log.c \
../src/headers/commons/process.c \
../src/headers/commons/string.c \
../src/headers/commons/temporal.c \
../src/headers/commons/txt.c 

OBJS += \
./src/headers/commons/bitarray.o \
./src/headers/commons/config.o \
./src/headers/commons/error.o \
./src/headers/commons/log.o \
./src/headers/commons/process.o \
./src/headers/commons/string.o \
./src/headers/commons/temporal.o \
./src/headers/commons/txt.o 

C_DEPS += \
./src/headers/commons/bitarray.d \
./src/headers/commons/config.d \
./src/headers/commons/error.d \
./src/headers/commons/log.d \
./src/headers/commons/process.d \
./src/headers/commons/string.d \
./src/headers/commons/temporal.d \
./src/headers/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
src/headers/commons/%.o: ../src/headers/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2014-1c-out-of-range/commons" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


