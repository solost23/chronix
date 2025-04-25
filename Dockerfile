FROM ubuntu:22.04 AS builder 

WORKDIR /build 

RUN apt update && \
    apt install -y gcc g++ make cmake && \
    apt install -y libssl-dev 

RUN apt update && \
    apt install -y tzdata && \
    rm -rf /var/lib/apt/lists/*
RUN ln -fsn /usr/share/zoneinfo/Asia/Shanghai /etc/localtime && \
    echo Asia/Shanghai > /etc/timezone

COPY . /build
RUN chmod +x /build/example.sh && \
    /build/example.sh 

FROM ubuntu:22.04

WORKDIR /app 

COPY --from=builder /build/lib /app/lib 
COPY --from=builder /build/bin/example /app/example

COPY --from=builder /etc/localtime /etc/localtime
COPY --from=builder /etc/timezone /etc/timezone

# 自动判断架构并设定 LD_LIBRARY_PATH
CMD ["/bin/bash", "-c", "\
ARCH=$(uname -m) && \
if [ \"$ARCH\" = \"x86_64\" ]; then \
  export LD_LIBRARY_PATH=/app/lib/mysqlx_x86_64; \
elif [ \"$ARCH\" = \"aarch64\" ] || [ \"$ARCH\" = \"arm64\" ]; then \
  export LD_LIBRARY_PATH=/app/lib/mysqlx_arm64; \
else \
  echo 'Unsupported architecture: '$ARCH && exit 1; \
fi && \
exec /app/example"]
