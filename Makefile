default: main_bin

main_bin: main.cc
	g++ -std=c++1z -O3 -o monocarlo main.cc

run: main_bin
	./monocarlo
