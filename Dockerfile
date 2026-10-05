# ---------- build stage ----------
FROM ubuntu:24.04 AS build
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        build-essential cmake ninja-build ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DAICAN_BUILD_TESTS=OFF -DAICAN_BUILD_EXAMPLES=OFF \
 && cmake --build build --parallel

# ---------- runtime stage ----------
FROM ubuntu:24.04
RUN useradd --system --uid 10001 --no-create-home aican
COPY --from=build /src/build/aican /usr/local/bin/aican
USER aican
WORKDIR /data
# Mount your logs at /data:  docker run --rm -v "$PWD/logs:/data:ro" ai-can-analyzer demo_attack.log
ENTRYPOINT ["aican"]
