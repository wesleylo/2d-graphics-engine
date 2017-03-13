################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GBitmap.cpp \
../src/GCanvas.cpp \
../src/GShader.cpp \
../src/GTime.cpp 

OBJS += \
./src/GBitmap.o \
./src/GCanvas.o \
./src/GShader.o \
./src/GTime.o 

CPP_DEPS += \
./src/GBitmap.d \
./src/GCanvas.d \
./src/GShader.d \
./src/GTime.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/weslo/workspace/pa6/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


