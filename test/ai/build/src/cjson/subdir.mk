################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cjson/cJSON.c 

OBJS += \
./src/cjson/cJSON.o 

C_DEPS += \
./src/cjson/cJSON.d 


# Each subdirectory must supply rules for building sources it contributes
src/cjson/%.o: ../src/cjson/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


