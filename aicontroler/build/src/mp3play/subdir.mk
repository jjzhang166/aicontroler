################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mp3play/mp3play.c 

OBJS += \
./src/mp3play/mp3play.o 

C_DEPS += \
./src/mp3play/mp3play.d 


# Each subdirectory must supply rules for building sources it contributes
src/mp3play/%.o: ../src/mp3play/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


