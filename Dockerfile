# build stage
ARG ALPINE_VERSION
FROM alpine:${ALPINE_VERSION} AS builder
# intstall git build-base
RUN apk add --no-cache git build-base cmake
# build app
COPY . /src
WORKDIR /src
RUN cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release && \
    cmake --build ./build --parallel $(nproc) && \
    GTEST_COLOR=TRUE ctest --test-dir ./build/test --output-on-failure && \
    cmake --install ./build

# final stage
ARG ALPINE_VERSION
FROM alpine:${ALPINE_VERSION}
# image info
ARG AUTHOR_NAME
ARG AUTHOR_EMAIL
ARG VERSION
# label
LABEL author=${AUTHOR_NAME} email=${AUTHOR_EMAIL} alpine_version=${ALPINE_VERSION} version=${VERSION}
# cpoy the app from builder
COPY --from=builder /usr/local/include/neujson /usr/local/include/neujson
COPY --from=builder /usr/local/lib/cmake/neujson /usr/local/lib/cmake/neujson
COPY --from=builder /usr/local/lib/pkgconfig/neujson.pc /usr/local/lib/pkgconfig/neujson.pc
