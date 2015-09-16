all:

clean:
	$(RM) -rf ./llvm/

install_llvm: script/setup_llvm.sh
	./$^
