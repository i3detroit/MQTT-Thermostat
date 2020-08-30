
-include local.mk
# local.mk should define PLATFORMIO_BUILD_FLAGS

.PHONY: help
help:   ## Show this help.
	@awk 'BEGIN {FS = ":.*##"; printf "Usage: make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

.PHONY: flash
flash:  ## tell pio to flash the board with settings from local.mk
	@PLATFORMIO_BUILD_FLAGS="$(PLATFORMIO_BUILD_FLAGS)" pio run -t upload

.PHONY: build
build:  ## tell pio to build  with settings from local.mk
	@PLATFORMIO_BUILD_FLAGS="$(PLATFORMIO_BUILD_FLAGS)" pio run
