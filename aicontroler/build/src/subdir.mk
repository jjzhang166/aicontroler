################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ai.c \
../src/control.c \
../src/err_log.c \
../src/main.c \
../src/record.c \
../src/rectime.c \
../src/writefile.c 

OBJS += \
./src/ai.o \
./src/control.o \
./src/err_log.o \
./src/main.o \
./src/record.o \
./src/rectime.o \
./src/writefile.o 

C_DEPS += \
./src/ai.d \
./src/control.d \
./src/err_log.d \
./src/main.d \
./src/record.d \
./src/rectime.d \
./src/writefile.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


