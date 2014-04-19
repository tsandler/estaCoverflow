################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/headers/commons/collections/dictionary.c \
../src/headers/commons/collections/list.c \
../src/headers/commons/collections/queue.c 

OBJS += \
./src/headers/commons/collections/dictionary.o \
./src/headers/commons/collections/list.o \
./src/headers/commons/collections/queue.o 

C_DEPS += \
./src/headers/commons/collections/dictionary.d \
./src/headers/commons/collections/list.d \
./src/headers/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/headers/commons/collections/%.o: ../src/headers/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


