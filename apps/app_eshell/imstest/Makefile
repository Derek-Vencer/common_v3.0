cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj-y := $(obj_cpp:.cpp=.o) $(obj_c:.c=.o)

subdir-ccflags-y += \
					 -Inet/lwip_2.1.2/src/include/ \
					 -Inet/lwip_2.1.2/src/include/lwip/ \
					 -Inet/lte/src/ \
					 -Inet/lte/src/ril/include/ \
					 -Inet/lte/src/ril/include/telephony/ \
					 -Inet/lte/src/ril/include/libril/ \
					 -Inet/ping/
ifeq ($(IMS_ON_CP),1)
subdir-ccflags-y += -DIMS_ON_CP
endif
