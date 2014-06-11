################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src3/kernel.c \
../src3/planificador_corto_plazo.c \
../src3/planificador_largo_plazo.c \
../src3/sockets.c 

OBJS += \
./src3/kernel.o \
./src3/planificador_corto_plazo.o \
./src3/planificador_largo_plazo.o \
./src3/sockets.o 

C_DEPS += \
./src3/kernel.d \
./src3/planificador_corto_plazo.d \
./src3/planificador_largo_plazo.d \
./src3/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src3/%.o: ../src3/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2014-1c-out-of-range/commons" -O0 -g3 -Wall -lrt -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


