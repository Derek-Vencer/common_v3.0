cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))

ccflags-y := -Iutils/rom_utils \
             -Iutils/boot_struct \
             -Iplatform/drivers/ana \
             -Iutils/secure_boot/inc \
             -Iutils/rom_utils \
             -Iservices/bt_app

CFLAGS_user_secure_boot.o := -O0

#USER_SECURE_BOOT_ROM_VERIFY ?= 1
ifeq ($(USER_SECURE_BOOT_ROM_VERIFY),1)
CFLAGS_user_secure_boot.o += -DUSER_SECURE_BOOT_ROM_VERIFY
else
ifeq ($(NO_SEC_ENG),1)
CFLAGS_user_secure_boot.o += -DNO_SEC_ENG
endif
endif

ifeq ($(SECURE_BOOT),1)
ccflags-y += -DSECURE_BOOT

ccflags-y += -Iservices/norflash_api
endif

lib_user_secure_boot-y := $(obj-y:.c=.o)
obj-y := lib_user_secure_boot.a
