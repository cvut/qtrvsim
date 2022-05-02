include .dev-config.mk

.dev-config.mk:
	[ ! -f .dev-config.mk ] && cp .dev-config.mk.default .dev-config.mk

########################################################################################################################
# WASM
########################################################################################################################

wasm:
	bash extras/building/build-wasm.sh $(EMSCRIPTEN_VERSION) $(QT_WASM_PATH) $(EMSDK_PATH)

wasm-clean:
	rm -rf $(WASM_BUILD_DIR)

wasm-clean-deep: wasm-clean
	rm -rf $(EMSDK_PATH)/upstream/cache/*
	rm -rf $(EMSDK_PATH)/upstream/emscripten/cache/*

wasm-serve:
	cp data/wasm/* $(WASM_BUILD_DIR)/target
	# Server is necessary due to CORS
	cd $(WASM_BUILD_DIR)/target/ && python -m http.server
	open localhost:8000

wasm-install-qt:
	python -m aqt install $(QT_WASM_VERSION) linux desktop wasm_32 --outputdir $(QT_INSTALL_DIR)