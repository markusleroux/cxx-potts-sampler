FROM alpine:3.12.3

RUN apk update && \
    apk upgrade && \
    apk --update --no-cache add \
        vim \
        catch2 \
        build-base \
        cmake \
        bash \
        libstdc++ \
        boost-dev

COPY . /sampler
WORKDIR /sampler

RUN cd build/ && \
    cmake .. && \
    cmake --build .

ENTRYPOINT ["bin/sampler"]
CMD [ "-n", "10", \
      "-t", "cycle", \
      "-B", "0.95", \
      "-q", "7", \
      "-d", "3" ]
