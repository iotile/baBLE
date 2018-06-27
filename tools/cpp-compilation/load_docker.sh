#!/usr/bin/env bash

INIT_DIR=`pwd`

cd `dirname "$0"`

if [ -f ${DOCKER_CACHE_FILE} ]; then
  gunzip -c ${DOCKER_CACHE_FILE} | docker load;
else
  mkdir -p ${DOCKER_CACHE_DIR}
  docker build -t ${DOCKER_IMAGE_NAME} .
  docker save ${DOCKER_IMAGE_NAME} | gzip > ${DOCKER_CACHE_FILE}
fi

cd ${INIT_DIR}
