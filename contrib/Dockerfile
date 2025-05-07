FROM amd64/ubuntu:20.04 AS base

#If you found this docker image helpful please donate TENZ to the maintainer
LABEL maintainer="RV9zdNeUTQUToZUcRp9uNF8gwH5LzDFtan"

EXPOSE 8766/tcp
EXPOSE 8767/tcp

ENV DEBIAN_FRONTEND=noninteractive

#Install runtime dependencies
RUN apt-get update && \
	apt-get install -y --no-install-recommends \
	bash wget net-tools libminiupnpc17 \
	libevent-2.1 libevent-pthreads-2.1 \
	libboost-system1.71 libboost-filesystem1.71 libboost-chrono1.71 \
	libboost-program-options1.71 libboost-thread1.71 \
	libzmq5 && \
	apt-get clean

FROM base AS build

#Install build dependencies
RUN apt-get update && \
	apt-get install -y --no-install-recommends \
	bash net-tools build-essential libtool autotools-dev automake \
	pkg-config libssl-dev libevent-dev bsdmainutils python3 \
	libboost-system1.71-dev libboost-filesystem1.71-dev libboost-chrono1.71-dev \
	libboost-program-options1.71-dev libboost-test1.71-dev libboost-thread1.71-dev \
	libzmq3-dev libminiupnpc-dev && \
	apt-get clean

#Copy source dir
COPY . /home/tenzura/build/Tenzura/
WORKDIR /home/tenzura/build/Tenzura

#build db4 from source
WORKDIR /home/tenzura/build/Tenzura/contrib
RUN ./install_db4.sh ../../

# build Tenzuracore
WORKDIR /home/tenzura/build/Tenzura
RUN ./autogen.sh && ./configure --disable-tests BDB_LIBS="-L/home/tenzura/build/db4/lib -ldb_cxx-4.8" BDB_CFLAGS="-I/home/tenzura/build/db4/include" --with-gui=no  && make -j4

FROM base AS final

#Add our service account user
RUN useradd -ms /bin/bash tenzura && \
	mkdir /var/lib/tenzura && \
	chown tenzura:tenzura /var/lib/tenzura && \
	ln -s /var/lib/tenzura /home/tenzura/.tenzura && \
	chown -h tenzura:tenzura /home/tenzura/.tenzura

VOLUME /var/lib/tenzura

#Copy the compiled binaries from the build
COPY --from=build /home/tenzura/build/Tenzura/src/tenzurad /usr/local/bin/tenzurad
COPY --from=build /home/tenzura/build/Tenzura/src/tenzura-cli /usr/local/bin/tenzura-cli

WORKDIR /home/tenzura
USER tenzura

CMD /usr/local/bin/tenzurad -datadir=/var/lib/tenzura -printtoconsole -onlynet=ipv4
