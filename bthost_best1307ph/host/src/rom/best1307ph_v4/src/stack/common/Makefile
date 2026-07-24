
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)patch/*.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)l2cap/*.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)gatt/*.c))

obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)utils/*.c))

src_obj := $(obj_c:.c=.o)

obj-y := $(src_obj)

ccflags-y += -DBLUETOOTH_BT_IMPL

ccflags-y += \
    $(BTH_STACK_INCLUDES)
