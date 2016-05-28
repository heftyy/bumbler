cmake_minimum_required(VERSION 3.1)

set(SHARED_SOURCES
  communication/message_settings.h
  communication/serializable_types.h
  communication/server_status.h
  communication/server_status.cpp
  )

add_library(shared STATIC ${SHARED_SOURCES})
