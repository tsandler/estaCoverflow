################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../parser/parser-dummy/dummy_ansisop.c \
../parser/parser-dummy/runner.c 

OBJS += \
./parser/parser-dummy/dummy_ansisop.o \
./parser/parser-dummy/runner.o 

C_DEPS += \
./parser/parser-dummy/dummy_ansisop.d \
./parser/parser-dummy/runner.d 


# Each subdirectory must supply rules for building sources it contributes
parser/parser-dummy/%.o: ../parser/parser-dummy/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


