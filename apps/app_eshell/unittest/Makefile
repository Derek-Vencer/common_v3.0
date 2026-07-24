cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))

obj-y := $(obj_cpp:.cpp=.o) $(obj_c:.c=.o)

CFLAGS_lowpower.o += -Iapps/sensorhub/

# =============
# beco.cpp
# =============
ifeq ($(filter 1, $(ENABLE_BECO) $(BECO)), 1)
    CFLAGS_beco.o += -DENABLE_BECO
    CFLAGS_beco.o += -Iinclude/beco
    CFLAGS_beco.o += -Iinclude/beco/beco_nn
endif

# =============
# ram.c
# =============
ESHELL_RAM_TEST ?= 1
ifeq ($(ESHELL_RAM_TEST), 1)
    CFLAGS_ram.o += -DESHELL_RAM_TEST
endif

# =============
# framebuf.c
# =============
ifneq ($(FB_BUFFER_HEIGHT),)
    CFLAGS_framebuf.o += -DFB_BUFFER_HEIGHT=$(FB_BUFFER_HEIGHT)
endif
ifneq ($(FB_BUFFER_WIDTH),)
    CFLAGS_framebuf.o += -DFB_BUFFER_WIDTH=$(FB_BUFFER_WIDTH)
endif
ifneq ($(FB_BUFFER_BPP),)
    CFLAGS_framebuf.o += -DFB_BUFFER_BPP=$(FB_BUFFER_BPP)
endif

# =============
# jpeg.c
# =============
CFLAGS_jpeg.o += \
    -Itests/display_test/ \
    -Iplatform/drivers/jpeg/ \
    -Iplatform/drivers/graphic/gpu/drv/vglite/inc/

# =============
# usb.c
# =============
ifeq ($(USB_TEST_ENABLE), 1)
    USB_TEST_CFLAGS += \
        -DUSB_TEST_ENABLE \
        -Iplatform/drivers/usb/usb_dev/inc/ \
        -Iplatform/drivers/usb/usb_host/inc/
    ifeq ($(MTP_ENABLE), 1)
        USB_TEST_CFLAGS += -DMTP_ENABLE \
        -Iservices/usb/usb_dev/mtp/ \
        -Iservices/usb/usb_host/msd/
    endif

    ifeq ($(USBHOST_MASS_ENABLE), 1)
        USB_TEST_CFLAGS += -DUSBHOST_MASS_ENABLE
    endif
    ifneq ($(CHIP_HAS_USB)-$(USB_HOST), 1-1)
        $(error UNITTEST USB TEST need CHIP_HAS_USB and USB_HOST)
    endif

    CFLAGS_usb.o += $(USB_TEST_CFLAGS)
endif

# =============
# blend.c
# =============
ifeq ($(TEST_WITH_GPU), 1)
    CFLAGS_gpu_blend.o += -DTEST_WITH_GPU \
        -Iplatform/drivers/graphic/gpu/drv/vglite/inc/ \
        -Iplatform/drivers/graphic/gpu/
endif


subdir-ccflags-y += \
    -Iapps/app_eshell \
    -Iplatform/hal \
    -Iplatform/drivers/ana \
    -Itests/hwtest \
    -Iutils/crc \

ifeq ($(CHIP_HAS_EMMC),1)
subdir-ccflags-y += -DCHIP_HAS_EMMC
subdir-ccflags-y += \
    -Iplatform/drivers/emmc/host/inc \
    -Iplatform/drivers/emmc/host/card \
    -Iplatform/drivers/emmc/host/core \
    -Iplatform/drivers/emmc/host/hci
endif

ifeq ($(RTC_CALENDAR),1)
subdir-ccflags-y += -DRTC_CALENDAR
endif

export I3C_TEST_ENABLE ?= 1
ifeq ($(I3C_TEST_ENABLE),1)
subdir-ccflags-y += -DI3C_TEST_ENABLE
endif

ifeq ($(BTH_SUBSYS_ONLY),1)
subdir-ccflags-y += -DBTH_SUBSYS_ONLY
endif

ifeq ($(ENABLE_MBEDTLS),1)
subdir-ccflags-y += -DENABLE_MBEDTLS
ifeq ($(SHANHAI_MBEDTLS_SUPPORT),1)
subdir-ccflags-y += -DSHANHAI_MBEDTLS
subdir-ccflags-y += \
    -Iutils/hexdump \
    -Iutils/WjCryptLib/lib \
    -Ithirdparty/shanhai/pal \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/configs \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library/dubhe_driver \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library/dubhe_driver/inc \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library/dubhe_driver/inc/common \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library/dubhe_driver/inc/crypto \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/include \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/include/mbedtls \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/include/psa \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library/bcm_driver \
    -Ithirdparty/shanhai/$(SHANHAI_DRV_VER)/mbedtls/library/bcm_crypto_lib
else
subdir-ccflags-y += \
    -Iutils/hexdump \
    -Iutils/mbedtls/include \
    -Iutils/mbedtls/include/mbedtls\
    -Iutils/mbedtls/ports/include \
    -Iutils/mbedtls/ports/tests
endif
endif
