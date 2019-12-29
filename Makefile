PROJECT_NAME := moonlight


include $(IDF_PATH)/make/project.mk

ifdef CONFIG_WEB_DEPLOY_SF
WEB_SRC_DIR = $(shell pwd)/frontend
ifneq ($(wildcard $(WEB_SRC_DIR)/.*),)
$(eval $(call spiffs_create_partition_image,www,$(WEB_SRC_DIR),FLASH_IN_PROJECT))
else
$(error $(WEB_SRC_DIR)/dist doesn't exist. Please run 'npm run build' in $(WEB_SRC_DIR))
endif
endif
