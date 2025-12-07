FROM espressif/esp-matter@sha256:42e914411453ecab6df6f2ff4ba1bbc9eec085e4496393f4b134296b7460dceb

RUN apt-get update -y && apt-get install ssh -y

RUN echo ". /opt/espressif/esp-idf/export.sh > /dev/null 2>&1" >> /root/.bashrc \
    && echo ". /opt/espressif/esp-matter/export.sh > /dev/null 2>&1" >> /root/.bashrc \
    && echo "export LC_ALL=C.UTF-8" >> /root/.bashrc \
    && echo "export LANG=C.UTF-8" >> /root/.bashrc
