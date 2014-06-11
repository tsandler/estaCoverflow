################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src3/headers/commons/bitarray.c \
../src3/headers/commons/config.c \
../src3/headers/commons/error.c \
../src3/headers/commons/log.c \
../src3/headers/commons/process.c \
../src3/headers/commons/string.c \
../src3/headers/commons/temporal.c \
../src3/headers/commons/txt.c 

OBJS += \
./src3/headers/commons/bitarray.o \
./src3/headers/commons/config.o \
./src3/headers/commons/error.o \
./src3/headers/commons/log.o \
./src3/headers/commons/process.o \
./src3/headers/commons/string.o \
./src3/headers/commons/temporal.o \
./src3/headers/commons/txt.o 

C_DEPS += \
./src3/headers/commons/bitarray.d \
./src3/headers/commons/config.d \
./src3/headers/commons/error.d \
./src3/headers/commons/log.d \
./src3/headers/commons/process.d \
./src3/headers/commons/string.d \
./src3/headers/commons/temporal.d \
./src3/headers/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
src3/headers/commons/%.o: ../src3/headers/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2014-1c-out-of-range/commons" -O0 -g3 -Wall -lrt -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


