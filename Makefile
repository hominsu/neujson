AUTHOR_NAME=hominsu
AUTHOR_EMAIL=1774069959@qq.com
VERSION=$(shell git describe --tags --always)

REPO=hominsu
APP_NAME=$(shell basename $$PWD)
DOCKER_IMAGE=$(REPO)/$(APP_NAME)-alpine:$(VERSION)

.PHONY: build
# build
build:
	cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release && \
    cmake --build ./build --parallel $(shell nproc)

.PHONY: docker
# build image
docker:
	docker build \
	--build-arg AUTHOR_NAME=$(AUTHOR_NAME) \
	--build-arg AUTHOR_EMAIL=$(AUTHOR_EMAIL) \
	--build-arg VERSION=$(VERSION) \
	--file Dockerfile \
	--tag $(DOCKER_IMAGE) .

.PHONY: buildx
# build multi-platform images
buildx:
	docker buildx build \
	--build-arg AUTHOR_NAME=$(AUTHOR_NAME) \
	--build-arg AUTHOR_EMAIL=$(AUTHOR_EMAIL) \
	--build-arg VERSION=$(VERSION) \
	--file Dockerfile \
	--platform=linux/arm64,linux/amd64,linux/ppc64le,linux/s390x,linux/386,linux/arm/v7,linux/arm/v6 \
	--push \
	--tag $(DOCKER_IMAGE) .

# show help
help:
	@echo ''
	@echo 'Usage:'
	@echo ' make [target]'
	@echo ''
	@echo 'Targets:'
	@awk '/^[a-zA-Z\-\_0-9]+:/ { \
	helpMessage = match(lastLine, /^# (.*)/); \
			if (helpMessage) { \
					helpCommand = substr($$1, 0, index($$1, ":")-1); \
					helpMessage = substr(lastLine, RSTART + 2, RLENGTH); \
					printf "\033[36m%-22s\033[0m %s\n", helpCommand,helpMessage; \
			} \
	} \
	{ lastLine = $$0 }' $(MAKEFILE_LIST)

.DEFAULT_GOAL := help