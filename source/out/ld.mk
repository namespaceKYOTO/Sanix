VPATH += ./../source/out

obj += skernel.o
obj += $(C32_OBJE)
obj += fasm.o inthandler.o

_OBJE_ALL = $(addprefix $(INTERMEDIATE)/,$(obj))

#-oformat -i
#--file-alignment 010200
LDFLAGS = -T
link :
	ld -o $(OUT_IMG)/Sanix -L $(OUT_OBJ) $(_OBJE_ALL) \
	$(LDFLAGS) $(INTERMEDIATE)/link.ld
	objcopy -R .note -R .comment -S -O binary $(OUT_IMG)/Sanix $(OUT_BIN)/Sanix.bin
	objdump -D -b elf32-i386 -m i386 $(OUT_IMG)/SIPL > $(OUT_DUMP)/Sanix_dump.txt
