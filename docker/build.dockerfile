ARG DEBIAN_RELEASE=forky
FROM debian:${DEBIAN_RELEASE} AS nmbs-build

RUN apt-get update && apt-get install --no-install-recommends --yes \
    build-essential cmake debhelper help2man libgtest-dev doxygen libargparse-dev libexiv2-dev libxml2-dev pkg-config libglib2.0-dev libnautilus-extension-dev

WORKDIR /usr/src/nmbs/src
COPY . .

RUN dpkg-buildpackage


FROM debian:trixie AS nmbs-trixie
RUN --mount=type=bind,from=nmbs-build,source=/usr/src/nmbs,target=/tmp/nmbs \
    apt-get update &&  \
    apt-get install --no-install-recommends --yes /tmp/nmbs/libnmbs1_1*.deb /tmp/nmbs/nmbs_1*.deb &&  \
    rm -rf /var/lib/apt/lists/* && \
    useradd -ms /bin/bash nmbs
USER nmbs


# Running this as a seperate target to allow flexibility with dependencies during the testing package transitions.
FROM debian:forky AS nmbs-forky
RUN --mount=type=bind,from=nmbs-build,source=/usr/src/nmbs,target=/tmp/nmbs \
    apt-get update &&  \
    apt-get install --no-install-recommends --yes /tmp/nmbs/libnmbs1_1*.deb /tmp/nmbs/nmbs_1*.deb &&  \
    rm -rf /var/lib/apt/lists/* && \
    useradd -ms /bin/bash nmbs
USER nmbs

