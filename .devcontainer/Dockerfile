FROM espressif/esp-matter

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update -y && apt-get install udev -y

RUN echo ". /opt/espressif/esp-idf/export.sh > /dev/null 2>&1" >> /root/.bashrc \
    && echo ". /opt/espressif/esp-matter/export.sh > /dev/null 2>&1" >> /root/.bashrc
