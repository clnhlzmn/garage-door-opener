FROM espressif/esp-matter

RUN apt-get update -y && apt-get install ssh -y

RUN echo ". /opt/espressif/esp-idf/export.sh > /dev/null 2>&1" >> /root/.bashrc \
    && echo ". /opt/espressif/esp-matter/export.sh > /dev/null 2>&1" >> /root/.bashrc
