####################################################################################################
#define the software version
#for example:
#BUILDVERSION = demo_app
#####################################################################################################
BUILDVERSION = V1.0-A

#####################################################################################################
#configure the object file is debug or release version
#for example:
#VERSIONTYPE = yes(debug version) or no(release version)
#####################################################################################################
VERSIONTYPE = yes

#####################################################################################################
#define the using macros in code when compiling,while different macro is separated by space
#for example:
#COMPILEMACRO := APP_DEBUG
#####################################################################################################
COMPILEMACRO :=

#####################################################################################################
#define the using compileling switchs when compiling,while different switch is separated by space
#for example:
#COMPILESWITCH := -g -O2
#####################################################################################################
ifeq ($(MAKECMDGOALS),arm)
COMPILESWITCH := -O3 -static -ffloat-store -fforce-addr -Wchar-subscripts -Wcomment -Wparentheses -Wreturn-type -Wswitch -Wunused -Wuninitialized -Wsign-compare -Wredundant-decls
else
COMPILESWITCH := -gdwarf-2 -O0 -static -ffloat-store -fforce-addr -Wchar-subscripts -Wcomment -Wparentheses -Wreturn-type -Wswitch -Wunused -Wuninitialized -Wsign-compare -Wredundant-decls
endif

ifeq (yes, $(VERSIONTYPE))
COMPILESWITCH += -g -fkeep-inline-functions
COMPILEMACRO  += DEBUGVERSION
COMPILEMACRO  += DETECTMEMOVERFLOWMACRO
else ifeq (YES, $(VERSIONTYPE))
COMPILESWITCH += -g -fkeep-inline-functions
COMPILEMACRO  += DEBUGVERSION
COMPILEMACRO  += DETECTMEMOVERFLOWMACRO
else
COMPILESWITCH += -finline-functions
COMPILEMACRO  += RELEASEVERSION
endif

ifeq ($(MAKECMDGOALS),arm)
#compiler := arm-linux-gcc
#compiler := /home/public/bio2000/arm-linux4.7/bin/arm-linux-gnueabihf-g++
compiler := /home/public/bio2000/zlg-arm-linux4.5/bin/arm-none-linux-gnueabi-g++ 
#COMPILEMACRO += COMPILER_IS_ARM_LINUX_GCC APP_ENABLE_PRE_ACT_PROTECT APP_XC8001_MIDMACHINE
COMPILEMACRO += COMPILER_IS_ARM_LINUX_GCC APP_ENABLE_PRE_ACT_PROTECT APP_XC8001_MIDMACHINE 
else
compiler := g++ 
COMPILEMACRO += COMPILER_IS_LINUX_GCC APP_ENABLE_PRE_ACT_PROTECT APP_XC8001_MIDMACHINE SIMUTEST 
endif
