ROOT		= /c/GitHub/project/Sanix

SOURCE		= $(ROOT)/source
TOOLS		= $(ROOT)/tools


NASM		= $(TOOLS)/nasm/nasm.exe
DISNASM		= .\tools\nasm\ndisasm.interruptexe
EDIMG		= $(TOOLS)/edimg/edimg.exe
IN_BIN		= $(SOURCE)
IN_BOOT		= $(SOURCE)/boot
IN_ASM		= $(SOURCE)/asm
IN_NASM		= $(SOURCE)/nasm
INCLUDE		= $(SOURCE)/include/
OUT			= ./out
OUT_BIN		= $(OUT)/bin
OUT_LISTING	= $(OUT)/listing
OUT_DUMP	= $(OUT)/dump
OUT_IMG		= $(OUT)/img
OUT_OBJ		= $(OUT)/obj
OUT_DUMP	= $(OUT)/dump
INTERMEDIATE	= $(SOURCE)/out
QEMU		= $(TOOLS)/qemu
DOXYGEN		= $(TOOLS)/doxygen


#--------------------------------------------------------
# ターゲット、必須項目を探すディレクトリを増やす
# target and required item
#--------------------------------------------------------
VPATH		= $(SOURCE) $(SOURCE)/graphic/ $(SOURCE)/boot/ \
			$(SOURCE)/asm/ $(SOURCE)/interrupt/ \
			$(SOURCE)/kernel/ $(SOURCE)/arc/x86 $(SOURCE)/out \
			$(SOURCE)/mem $(SOURCE)/nasm $(SOURCE)/debug \
			$(SOURCE)/include $(SOURCE)/math/ $(SOURCE)/driver/ \
			$(SOURCE)/util $(SOURCE)/ui $(SOURCE)/arc/x86 \
			$(SOURCE)/driver/ata $(SOURCE)/arc/x86/pci \
			$(SOURCE)/arc/x86/include $(SOURCE)/sys \
			$(SOURCE)/console

_DEF = -D__ENABLE_PROCESS__ -D__SDEBUG__
_INC = -I $(SOURCE)/include -I $(SOURCE)/arc/x86/include -finline-functions \
		-include $(SOURCE)/include/type.h -include $(SOURCE)/include/define.h
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
# OS Image Sanix
#--------------------------------------------------------
Sanix.img : $(OUT_BIN)/bootloader.bin $(OUT_BIN)/Sanix.bin
	@echo ++-+-+-+-+-+-+-+-+-++
	@echo     create .img
	@echo ++-+-+-+-+-+-+-+-+-++
	cat $(OUT_BIN)/bootloader.bin $(OUT_BIN)/bs.bin > $(OUT_BIN)/boot.bin
	cat $(OUT_BIN)/boot.bin $(OUT_BIN)/Sanix.bin > $(OUT_IMG)/$@
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
	-I $(SOURCE)/include -nostdlib \
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
#	-make c32_dump

skernel.o : skernel.c
	@echo ---------------
	@echo   $@
	@echo ---------------
	gcc -c $< -o $(OUT_OBJ)/$@ \
	$(CFLAGS)

#--------------------------------------------------------
# intermediate file Sanix
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
	chmod 777 -R $(ROOT)/source

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
	$(MAKE) Sanix.img

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

