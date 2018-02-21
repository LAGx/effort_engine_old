

export VULKAN_SDK
COMPILER = $(VULKAN_SDK)/bin/glslangValidator


all: shader  
	

shader:
	$(COMPILER) -V $(FILE_TO_COMPILE) -o $(FILE_OUTPUT) || exit 1

