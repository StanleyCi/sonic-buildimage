SAI_VER = 3.2.1.1
BRCM_SAI = libsaibcm_$(SAI_VER)_amd64.deb
$(BRCM_SAI)_PATH = platform/broadcom/DeltaDebs

BRCM_SAI_DEV = libsaibcm-dev_$(SAI_VER)_amd64.deb
$(eval $(call add_derived_package,$(BRCM_SAI),$(BRCM_SAI_DEV)))
$(BRCM_SAI_DEV)_PATH = platform/broadcom/DeltaDebs

SONIC_COPY_DEBS += $(BRCM_SAI) $(BRCM_SAI_DEV)
$(BRCM_SAI_DEV)_DEPENDS += $(BRCM_SAI)
