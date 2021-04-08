#####################################################################################################
#configure the user code source file path
#for example:
#SRCCODEDIRS := ../../code/src/com
#####################################################################################################
SRCCODEDIRS   :=../../code/src/proto \
                ../../code/src/vos \
				../../code/src/com \
                ../../code/src/env \
                ../../code/src/msg \
				../../code/src/utils \
                ../../code/src/schedule \
                ../../code/src/taskresult \
                ../../code/src/mnguart \
                ../../code/src/debugtty \
                ../../code/src/debugcmd \
                ../../code/src/taskmng \
                ../../code/src/oam \
                ../../code/src/subsys \
                ../../code/src/repdispose \
                ../../code/src/maintain \
                ../../code/src/simulation\
                ../../code/src/getinfo 


#####################################################################################################
#configure the user code header file path
#for example:
#SRCHEADDIRS := ../../code/inc/com
#####################################################################################################
SRCHEADDIRS   :=../../code/inc/proto \
                ../../code/inc/vos \
				../../code/inc/com \
                ../../code/inc/env \
                ../../code/inc/msg \
				../../code/inc/utils \
                ../../code/inc/schedule \
                ../../code/inc/taskresult \
                ../../code/inc/taskmng   \
                ../../code/inc/middwnmachshare \
                ../../code/inc/middwnmachshare/cooling \
                ../../code/inc/middwnmachshare/reaction \
                ../../code/inc/middwnmachshare/reagent \
                ../../code/inc/middwnmachshare/sample \
                ../../code/inc/mnguart \
                ../../code/inc/debugtty \
                ../../code/inc/debugcmd \
                ../../code/inc/taskmng \
                ../../code/inc/upmidmachshare \
                ../../code/inc/oam \
                ../../code/inc/subsys \
                ../../code/inc/repdispose \
                ../../code/inc/maintain \
                ../../code/inc/simulation \
                ../../code/inc/poweronselftest \
                ../../code/inc/getinfo 


#####################################################################################################
#configure the lib file path
#for example:
#LIBFILEDIRS := ../../lib/vos/lib
#####################################################################################################
LIBFILEDIRS := ../../lib/lib 

#####################################################################################################
#configure the lib header file path
#for example:
#LIBHEADDIRS := ../../lib/vos/inc
#####################################################################################################
LIBHEADDIRS := ../../lib/inc/drv \
               ../../lib/inc/protobuf

#####################################################################################################
#configure the lib file
#for example:
#LIBFILE := -lvos
#####################################################################################################
ifeq ($(MAKECMDGOALS),arm)
LIBFILE := -larmprotobuf -pthread -lrt
else
LIBFILE := -lprotobuf -pthread -lrt 
endif
