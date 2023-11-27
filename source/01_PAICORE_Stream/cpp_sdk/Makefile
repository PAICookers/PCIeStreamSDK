f := 00_work

uart:
	/home/cjailab/sw/Miniconda/envs/PAIFLOW/bin/python ../py_sdk/serial_utils.py

compile:
	g++ ./dma_utils.cpp ./${f} -o ./main -w

run: uart compile
	sudo ./main
