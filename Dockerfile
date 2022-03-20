# build stage
FROM alpine:latest AS builder
# intstall git build-base
RUN apk add --no-cache git build-base cmake
# build app
COPY . /src
WORKDIR /src
RUN cmake -DCMAKE_BUILD_TYPE=Release . && cmake --build . --parallel $(nproc) && cmake --install .

# final stage
FROM alpine:latest
# image info
ARG AUTHOR_NAME
ARG AUTHOR_EMAIL
ARG VERSION
# label
LABEL author=$AUTHOR_NAME email=${AUTHOR_EMAIL} version=$VERSION
# cpoy the app from builder
COPY --from=builder /usr/local/lib/libneujson.a /usr/local/lib/libneujson.a
COPY --from=builder /usr/local/include/neujson /usr/local/include/neujson
COPY --from=builder /usr/local/lib/cmake/neujson /usr/local/lib/cmake/neujson