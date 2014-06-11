################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Crawler.cpp \
../HTMLFileParser.cpp \
../Parser.cpp \
../ParserCommunicator.cpp \
../main.cpp 

OBJS += \
./Crawler.o \
./HTMLFileParser.o \
./Parser.o \
./ParserCommunicator.o \
./main.o 

CPP_DEPS += \
./Crawler.d \
./HTMLFileParser.d \
./Parser.d \
./ParserCommunicator.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -I"C:\Users\Thomas\Documents\GitHub\Thywin\source\thywinlib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


