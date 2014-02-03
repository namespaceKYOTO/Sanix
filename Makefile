ROOT		= /c/GitHub/project/Sanix
TOOLS		= $(ROOT)/tools


NASM		= $(TOOLS)/nasm/nasm.exe
DISNASM		= .\tools\nasm\ndisasm.interruptexe
EDIMG		= $(TOOLS)/edimg/edimg.exe
IN_BIN		= ./../source/in
IN_BOOT		= ./../source/in/boot
IN_ASM		= ./../source/in/asm
IN_NASM		= ./../source/in/nasm
INCLUDE		= ./../source/in/include/
OUT			= ./../source/out
OUT_BIN		= ./../source/out/bin
OUT_LISTING	= ./../source/out/listing
OUT_DUMP	= ./../source/out/dump
OUT_IMG		= ./../source/out/img
OUT_OBJ		= ./../source/out/obj
OUT_DUMP	= ./../source/out/dump
INTERMEDIATE	= ./../source/in/out
QEMU		= $(TOOLS)/qemu
DOXYGEN		= $(TOOLS)/doxygen


#################################################
# ターゲット、必須項目を探すディレクトリを増やす
# target and required item
#
VPATH		= ./in/ ./in/graphic/ ./in/boot/ ./in/asm/ \
			./in/interrupt/ ./in/kernel/ ./in/arc/x86 ./in/out \
			./in/mem ./in/nasm ./in/debug ./in/include \
			./in/math/ ./in/driver/ ./in/util ./in/ui ./in/arc/x86 \
			./in/driver/ata ./in/arc/x86/pci ./in/arc/x86/include \
			./in/sys ./in/console

_DEF = -D__ENABLE_PROCESS__ -D__SDEBUG__
_INC = -I ./in/include -I ./in/arc/x86/include -finline-functions \
		-include ./in/include/type.h -include ./in/include/define.h
CFLAGS =  $(_INC) \
		$(_DEF)

#デフォルト操作
default:
	$(MAKE) all

#--------------------------------------------------------
# NASM Object
#--------------------------------------------------------
%.bin : $(IN_BOOT)/%.nasm Makefile
	@echo ---------------
	@echo   $@
	@echo ---------------
	$(NASM) -f bin $(IN_BOOT)/$(<F) -o $(OUT_BIN)/$@ -l $(OUT_DUMP)/$*_dump.txt \
	-i $(INCLUDE)/ -i $(IN_NASM)/ -i $(IN_BOOT)/

%.o : $(IN_NASM)/%.nasm Makefile
	@echo ---------------
	@echo   $@
	@echo ---------------
	$(NASM) -f elf32 $(IN_NASM)/$(<F) -o $(OUT_BIN)/$@ -l $(OUT_DUMP)/$*_dump.txt \
	-i $(INCLUDE)/ -i $(IN_NASM)/
	cp $(OUT_BIN)/$@ $(INTERMEDIATE)/$@

NASM_OBJ := bootloader.bin empty.bin bs.bin fasm.o \
			inthandler.o

nasm_obj :
	make $(NASM_OBJ)


#--------------------------------------------------------
# OS Image SIPL
#--------------------------------------------------------
SIPL.img : $(OUT_BIN)/bootloader.bin $(OUT_BIN)/SIPL.bin
	@echo ++-+-+-+-+-+-+-+-+-++
	@echo     create .img
	@echo ++-+-+-+-+-+-+-+-+-++
#	cat $(OUT_BIN)/bootloader.bin $(OUT_BIN)/empty.bin $(OUT_BIN)/bs.bin > $(OUT_BIN)/boot.bin
	cat $(OUT_BIN)/bootloader.bin $(OUT_BIN)/bs.bin > $(OUT_BIN)/boot.bin
	cat $(OUT_BIN)/boot.bin $(OUT_BIN)/SIPL.bin > $(OUT_IMG)/$@
	cp $(OUT_IMG)/$@ $(QEMU)/hdimage0.bin
	-objdump -b binary -M intel -m i386 -D $(OUT_IMG)/$@ > $(OUT_DUMP)/$@_dump.txt
	@echo ++-+-+-+-+-+-+-+-+-++
	@echo  Success create .img
	@echo ++-+-+-+-+-+-+-+-+-++

#--------------------------------------------------------
# GAS Object
#--------------------------------------------------------
asm.o : $(GAS_OBJ)
	@echo ---------------
	@echo   $@
	@echo ---------------
	gcc -c $(GAS_OBJ) \
	-I ./in/include -nostdlib \
	-march=i386

GAS_OBJ := bootloader.bin empty.bin bs.bin fasm.o \
			inthandler.o

asm_obj :
	$(MAKE) asm.o
	-cp *.o $(INTERMEDIATE)
	-rm *.o
#	$(MAKE) asm_dump.txt

#_dump_obj := A20.o fasm16.o
#GAS_DUMP_OBJ = $(addprefix $(INTERMEDIATE)/,$(_dump_obj))
#
##pe-i386
#asm_dump.txt : $(_GAS_OBJ)
#	-objdump -b elf32-i386 -m i8086 -M intel -D $(GAS_DUMP_OBJ) > $(OUT_DUMP)/$@

#--------------------------------------------------------
# c language
#--------------------------------------------------------
%.o : %.c %.h
	@echo ---------------
	@echo   $@
	@echo ---------------
	gcc -c $< -o $(OUT_OBJ)/$@ \
	$(CFLAGS)

#elf32-i386
%.o_dump.txt : $(OUT_OBJ)/%.o
	-objdump -D -M intel -b elf32-i386 -m i8086 $< > $(OUT_DUMP)/$@

#+-+-+-+-+-+- 32bit mode -+-+-+-+-+-+-#
C32_OBJE := boot.o interrupt.o descriptor.o \
			define.o graphic.o debug.o \
			time.o task.o page.o memory.o \
			graphic2d.o graphic3d.o math.o \
			ata.o list.o lock.o util.o console.o \
			widget.o process.o pci.o signal.o \
			system.o

C32_DUMP := $(addsuffix _dump.txt,$(C32_OBJE))

c32_obj :
	@echo $(C32_OBJE)
	$(MAKE) $(C32_OBJE)

c32_dump :
	$(MAKE) $(C32_DUMP)

c32 :
	-make c32_obj
	@echo -------------------------
	@echo -------------------------
	-make c32_dump

skernel.o : skernel.c
	@echo ---------------
	@echo   $@
	@echo ---------------
	gcc -c $< -o $(OUT_OBJ)/$@ \
	$(CFLAGS)

#--------------------------------------------------------
# intermediate file SIPL
# link .o
#--------------------------------------------------------
include $(INTERMEDIATE)/ld.mk



#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
#+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
del :
	-rm -r -f $(OUT)
	-rm $(INTERMEDIATE)/*.o

dir :
	-mkdir $(OUT)
	-mkdir $(OUT_BIN)
	-mkdir $(OUT_DUMP)
	-mkdir $(OUT_IMG)
	-mkdir $(OUT_OBJ)
	chmod 777 -R $(ROOT)/in

bin :
	-mkdir $(OUT)
	-mkdir $(OUT_BIN)
	-mkdir $(OUT_DUMP)
	-make nasm_obj

obj :
	-mkdir $(OUT)
	-mkdir $(OUT_OBJ)
	@echo -------------------------
	@echo ------- Assembler -------
	@echo -------------------------
	$(MAKE) bin
	@echo -------------------------
	@echo -------- Compile --------
	@echo -------------------------
	$(MAKE) c32
	$(MAKE) skernel.o

compile :
	$(MAKE) obj
	cp $(OUT_OBJ)/*.o $(INTERMEDIATE)
	chmod 777 -R $(INTERMEDIATE)
	$(MAKE) link

full :
	$(MAKE) dir
	$(MAKE) compile 2>&1 | tee compile_log.txt
	$(MAKE) SIPL.img

rebuild:
	make del
	make full

launcher :
	$(MAKE) -C $(QEMU)

run :
	$(MAKE) full
	$(MAKE) launcher

docment:
	$(MAKE) -C $(DOXYGEN)

all : 
	@echo -------------------------
	@echo --- @1 File Operation ---
	@echo -------------------------
	$(MAKE) del
	$(MAKE) dir
	@echo -------------------------
	@echo ----- @2 Make .img  -----
	@echo -------------------------
	$(MAKE) full
	@echo -------------------------
	@echo ------ @3  run   --------
	@echo -------------------------
	$(MAKE) launcher

