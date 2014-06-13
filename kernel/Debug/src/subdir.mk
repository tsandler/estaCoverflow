################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/kernel.c \
../src/planificador_corto_plazo.c \
../src/planificador_largo_plazo.c \
../src/socketsKernel.c 

OBJS += \
./src/kernel.o \
./src/planificador_corto_plazo.o \
./src/planificador_largo_plazo.o \
./src/socketsKernel.o 

C_DEPS += \
./src/kernel.d \
./src/planificador_corto_plazo.d \
./src/planificador_largo_plazo.d \
./src/socketsKernel.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2014-1c-out-of-range/commons" -O0 -g3 -Wall -lrt -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


