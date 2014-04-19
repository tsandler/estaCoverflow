################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/src/headers/commons/collections/dictionary.c \
../kernel/src/headers/commons/collections/list.c \
../kernel/src/headers/commons/collections/queue.c 

OBJS += \
./kernel/src/headers/commons/collections/dictionary.o \
./kernel/src/headers/commons/collections/list.o \
./kernel/src/headers/commons/collections/queue.o 

C_DEPS += \
./kernel/src/headers/commons/collections/dictionary.d \
./kernel/src/headers/commons/collections/list.d \
./kernel/src/headers/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/src/headers/commons/collections/%.o: ../kernel/src/headers/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


