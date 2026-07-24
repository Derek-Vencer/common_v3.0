cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_c += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp += $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y += \
    pressure_core.o \
    press_detection/


ccflags-y +=  \
    -Iplatform/drivers/pressure/ \
    -Iservices/pressure/press_detection/ \
    -Iplatform/drivers/ana/
