cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

src_obj := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)a2dp/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)avrcp/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)conmgr/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hci/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)l2cap/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)me/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sdp/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)spp/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hfp/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hid/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)os/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)rfcomm/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sync/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)besaud/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)dip/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)pbap/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)map/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)pan/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)opp/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)lhdc_license/*.c))
src_obj += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)sco/*.c))

obj_s := $(filter %.S, $(src_obj))
obj_c := $(filter %.c, $(src_obj))
obj_cpp := $(filter %.cpp, $(src_obj))

src_obj := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

obj-y += $(src_obj)

ccflags-y += -DBLUETOOTH_BT_IMPL

ccflags-y += \
    $(BTH_STACK_INCLUDES)
