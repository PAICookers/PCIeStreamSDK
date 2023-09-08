
# variables for compile source code to exectable program:
CSRC +=  $(shell find $(abspath ./source) -name "*.cpp")
BUILD_DIR := build
TARGET := ${BUILD_DIR}/transfer_once.exe
CFLAGS :=
LDFLAGS := -lpthread

.PHONY: run clean

$(BUILD_DIR):
	mkdir $@

com:${BUILD_DIR} ${CSRC}
	@g++ ${CSRC} ${CFLAGS} ${LDFLAGS} -o ${TARGET}

run:
	@${TARGET} --mode=${mode} --cFrmNum=${cFrmNum} --iFrmNum=${iFrmNum} --wFrmNum=${wFrmNum} --oFrmNum=${oFrmNum} --cpath=${configpath} --ipath=${initpath} --wpath=${workpath} --opath=${outputpath}

clean:
	rm -rf ${BUILD_DIR}
