AUTHOR_NAME=hominsu
AUTHOR_EMAIL=hominsu@foxmail.com
VERSION=$(shell git describe --tags --always)
ALPINE_VERSION=3.16

REPO=hominsu
APP_NAME=$(shell basename $$PWD)
DOCKER_IMAGE=$(REPO)/$(APP_NAME)-alpine:alpine-$(ALPINE_VERSION)-$(VERSION)

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
	--build-arg  ALPINE_VERSION=$(ALPINE_VERSION) \
	--build-arg VERSION=$(VERSION) \
	--file Dockerfile \
	--tag $(DOCKER_IMAGE) .

.PHONY: bake
# build multi-platform images
bake:
	AUTHOR_NAME=$(AUTHOR_NAME) \
	AUTHOR_EMAIL=$(AUTHOR_EMAIL) \
	ALPINE_VERSION=$(ALPINE_VERSION) \
	VERSION=$(VERSION) \
	docker buildx bake \
	--file docker-bake.hcl \
	--load \
	--set "*.platform=linux/arm64,linux/amd64,linux/ppc64le,linux/s390x,linux/386,linux/arm/v7,linux/arm/v6"

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