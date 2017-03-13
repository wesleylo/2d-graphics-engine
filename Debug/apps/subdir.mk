################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../apps/GWindow.cpp \
../apps/draw.cpp \
../apps/image.cpp \
../apps/image_recs.cpp \
../apps/test_recs.cpp \
../apps/tests.cpp 

OBJS += \
./apps/GWindow.o \
./apps/draw.o \
./apps/image.o \
./apps/image_recs.o \
./apps/test_recs.o \
./apps/tests.o 

CPP_DEPS += \
./apps/GWindow.d \
./apps/draw.d \
./apps/image.d \
./apps/image_recs.d \
./apps/test_recs.d \
./apps/tests.d 


# Each subdirectory must supply rules for building sources it contributes
apps/%.o: ../apps/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/weslo/workspace/pa6/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


