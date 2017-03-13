################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../GCanvas.cpp \
../GShader.cpp \
../MyCanvas.cpp \
../MyShaderFromBitmap.cpp \
../MyShaderFromLinearGradient.cpp \
../MyShaderFromRadialGradient.cpp 

OBJS += \
./GCanvas.o \
./GShader.o \
./MyCanvas.o \
./MyShaderFromBitmap.o \
./MyShaderFromLinearGradient.o \
./MyShaderFromRadialGradient.o 

CPP_DEPS += \
./GCanvas.d \
./GShader.d \
./MyCanvas.d \
./MyShaderFromBitmap.d \
./MyShaderFromLinearGradient.d \
./MyShaderFromRadialGradient.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/weslo/workspace/pa6/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


