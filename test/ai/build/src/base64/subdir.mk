################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/base64/decode.c \
../src/base64/encode.c 

OBJS += \
./src/base64/decode.o \
./src/base64/encode.o 

C_DEPS += \
./src/base64/decode.d \
./src/base64/encode.d 


# Each subdirectory must supply rules for building sources it contributes
src/base64/%.o: ../src/base64/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


