include_directories(./include)

AUX_SOURCE_DIRECTORY(./src Detection_SRCS)
ADD_LIBRARY(Detection ${Detection_SRCS})

