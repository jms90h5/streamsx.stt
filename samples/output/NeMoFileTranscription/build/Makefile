# NOTE: This makefile is designed to be run from one level up in the directory
# hierarchy compared to where it resides. For instance, if this makefile is
# located at 'output/build/Makefile', then it can be launched as:
# 'make -f build/Makefile' from the 'output' directory.

SHELL=/bin/bash
ifeq ($(CXX),)
  CXX=g++
endif
ifeq ($(CXX_LN),)
  CXX_LN=g++
endif
PKGCONFIG := /homes/jsharpe/teracloud/streams/7.2.0.0/bin/dst-pe-pkg-config-alternative.sh
SPL_BUNDLE_BUILDER := /homes/jsharpe/teracloud/streams/7.2.0.0/system/impl/bin/spl-bundle-builder
SO_INCLUDE := $(shell $(PKGCONFIG) --cflags dst-spl-pe-install)
SO_LIB := $(shell $(PKGCONFIG) --libs dst-spl-pe-install)
SO_STAND_LIB := $(shell $(PKGCONFIG) --libs dst-spl-pe-install-standalone)
SPL_CXXFLAGS = -Wall -Wno-unused -Wno-sign-compare
SPL_CXXFLAGS += -D_REENTRANT -fPIC -finline-functions -fmessage-length=0
SPL_CXXFLAGS += $(SO_INCLUDE) -O3 -DNDEBUG  -std=gnu++17 
SPL_LDFLAGS = $(SO_LIB)

SPL_TYPES = build/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.o \
            build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o \
            build/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.o \
            build/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.o \
            build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
            build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o \
            build/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.o 
SPL_FUNCTIONS = 
SPL_OPERATORS = build/operator/ProcessedAudio.o \
                build/operator/NeMoOut.o \
                build/operator/Analysis.o \
                build/operator/DisplayAnalysis.o \
                build/operator/TranscriptionFile.o \
                build/operator/AnalysisCSV.o \
                build/operator/AudioStream/RawAudioBlocks.o \
                build/operator/AudioStream/AudioStream.o 
SPL_SO_BINS = bin/ProcessedAudio.so \
              bin/NeMoOut.so \
              bin/Analysis.so \
              bin/DisplayAnalysis.so \
              bin/TranscriptionFile.so \
              bin/AnalysisCSV.so \
              bin/AudioStream.RawAudioBlocks.so \
              bin/AudioStream.AudioStream.so 

SPL_ADL_FILE = NeMoFileTranscription.adl

SPL_OUTPUT_DIR_ROOT = $(shell printf '%q' '/homes/jsharpe/teracloud/com.teracloud.streamsx.stt/samples/output')
SPL_OUTPUT_DIR = $(shell printf '%q' 'NeMoFileTranscription')

.PHONY: distclean clean all types functions operators sos bundle toolkits standalone sablink

all: bin types functions operators sos standalone bundle sablink

distclean clean:
	@rm -fr bin/* build/type build/function build/operator build/bundle build/standalone NeMoFileTranscription.sab

bin:
	@mkdir -m 755 bin

types: $(SPL_TYPES)

build/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.o: \
	    src/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.h \
	    src/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] enum{mono8k,mono16k,mono22k,mono44k,mono48k}'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.cpp

build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o: \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] enum{csv,txt,bin,block,line}'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.cpp

build/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.o: \
	    src/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.h \
	    src/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] enum{WindowMarker,FinalMarker}'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.cpp

build/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.o: \
	    src/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.h \
	    src/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<rstring transcription>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.cpp

build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o: \
	    src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.h \
	    src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<blob data>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.cpp

build/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.o: \
	    src/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.h \
	    src/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<rstring filename,rstring transcription,int32 wordCo...e,timestamp endTime>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.cpp

build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o: \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.h \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.cpp \
	    build/cppOptions \
	    | build/type
	@echo ' [CXX-type] tuple<blob audioChunk,uint64 audioTimestamp>'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.cpp

build/type:
	-@mkdir -m 755 build/type

operators: $(SPL_OPERATORS)

-include build/operator/ProcessedAudio.d
build/operator/ProcessedAudio.o: \
	    src/operator/ProcessedAudio.h \
	    src/operator/ProcessedAudio.cpp\
	    build/dep/operator/ProcessedAudio.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] ProcessedAudio'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/ProcessedAudio.cpp

-include build/operator/NeMoOut.d
build/operator/NeMoOut.o: \
	    src/operator/NeMoOut.h \
	    src/operator/NeMoOut.cpp\
	    build/dep/operator/NeMoOut.dep \
	    src/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.h \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] NeMoOut'
	@$(CXX) -o $@ -MD -I ../../../impl/nl/include -I /homes/jsharpe/teracloud/com.teracloud.streamsx.stt/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/NeMoOut.cpp

-include build/operator/Analysis.d
build/operator/Analysis.o: \
	    src/operator/Analysis.h \
	    src/operator/Analysis.cpp\
	    build/dep/operator/Analysis.dep \
	    src/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.h \
	    src/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] Analysis'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/Analysis.cpp

-include build/operator/DisplayAnalysis.d
build/operator/DisplayAnalysis.o: \
	    src/operator/DisplayAnalysis.h \
	    src/operator/DisplayAnalysis.cpp\
	    build/dep/operator/DisplayAnalysis.dep  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] DisplayAnalysis'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/DisplayAnalysis.cpp

-include build/operator/TranscriptionFile.d
build/operator/TranscriptionFile.o: \
	    src/operator/TranscriptionFile.h \
	    src/operator/TranscriptionFile.cpp\
	    build/dep/operator/TranscriptionFile.dep \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] TranscriptionFile'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/TranscriptionFile.cpp

-include build/operator/AnalysisCSV.d
build/operator/AnalysisCSV.o: \
	    src/operator/AnalysisCSV.h \
	    src/operator/AnalysisCSV.cpp\
	    build/dep/operator/AnalysisCSV.dep \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h  \
	    build/cppOptions \
	    | build/operator
	@echo ' [CXX-operator] AnalysisCSV'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/AnalysisCSV.cpp

-include build/operator/AudioStream/RawAudioBlocks.d
build/operator/AudioStream/RawAudioBlocks.o: \
	    src/operator/AudioStream/RawAudioBlocks.h \
	    src/operator/AudioStream/RawAudioBlocks.cpp\
	    build/dep/operator/AudioStream/RawAudioBlocks.dep \
	    src/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.h  \
	    build/cppOptions \
	    | build/operator/AudioStream
	@echo ' [CXX-operator] AudioStream.RawAudioBlocks'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/AudioStream/RawAudioBlocks.cpp

-include build/operator/AudioStream/AudioStream.d
build/operator/AudioStream/AudioStream.o: \
	    src/operator/AudioStream/AudioStream.h \
	    src/operator/AudioStream/AudioStream.cpp\
	    build/dep/operator/AudioStream/AudioStream.dep \
	    src/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.h  \
	    build/cppOptions \
	    | build/operator/AudioStream
	@echo ' [CXX-operator] AudioStream.AudioStream'
	@$(CXX) -o $@ -MD -I /homes/jsharpe/teracloud/streams/7.2.0.0/toolkits/spl/impl/nl/include -DSPLJAVAFUNCTION_ADL_FILENAME=\"NeMoFileTranscription.adl\" -c $(SPL_CXXFLAGS) src/operator/AudioStream/AudioStream.cpp

build/operator/AudioStream:
	-@mkdir -m 755 -p build/operator/AudioStream

build/operator:
	-@mkdir -m 755 -p build/operator

functions: $(SPL_FUNCTIONS)

sos: $(SPL_SO_BINS)

SOProcessedAudio = \
	    build/operator/ProcessedAudio.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o 

bin/ProcessedAudio.so: \
	$(SOProcessedAudio) \
	build/dep/ProcessedAudio.so \
	build/ldOptions
	@echo ' [LD-so] so ProcessedAudio'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/ProcessedAudio.so -fPIC -shared $(SOProcessedAudio) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SONeMoOut = \
	    build/operator/NeMoOut.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o \
	    build/type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.o \
	    build/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.o 

bin/NeMoOut.so: \
	$(SONeMoOut) \
	build/dep/NeMoOut.so \
	build/ldOptions
	@echo ' [LD-so] so NeMoOut'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/NeMoOut.so -fPIC -shared $(SONeMoOut) -L /homes/jsharpe/teracloud/com.teracloud.streamsx.stt/impl/lib -Wl,-rpath -Wl,\$$ORIGIN/../toolkits/com.teracloud.streamsx.stt/impl/lib -lnemo_ctc_interface $(SPL_LDFLAGS)

SOAnalysis = \
	    build/operator/Analysis.o \
	    build/type/BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.o \
	    build/type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.o \
	    build/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.o 

bin/Analysis.so: \
	$(SOAnalysis) \
	build/dep/Analysis.so \
	build/ldOptions
	@echo ' [LD-so] so Analysis'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/Analysis.so -fPIC -shared $(SOAnalysis) $(SPL_LDFLAGS)

SODisplayAnalysis = \
	    build/operator/DisplayAnalysis.o \
	    build/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.o 

bin/DisplayAnalysis.so: \
	$(SODisplayAnalysis) \
	build/dep/DisplayAnalysis.so \
	build/ldOptions
	@echo ' [LD-so] so DisplayAnalysis'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/DisplayAnalysis.so -fPIC -shared $(SODisplayAnalysis) $(SPL_LDFLAGS)

SOTranscriptionFile = \
	    build/operator/TranscriptionFile.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.o 

bin/TranscriptionFile.so: \
	$(SOTranscriptionFile) \
	build/dep/TranscriptionFile.so \
	build/ldOptions
	@echo ' [LD-so] so TranscriptionFile'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/TranscriptionFile.so -fPIC -shared $(SOTranscriptionFile) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SOAnalysisCSV = \
	    build/operator/AnalysisCSV.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.o 

bin/AnalysisCSV.so: \
	$(SOAnalysisCSV) \
	build/dep/AnalysisCSV.so \
	build/ldOptions
	@echo ' [LD-so] so AnalysisCSV'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AnalysisCSV.so -fPIC -shared $(SOAnalysisCSV) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SOAudioStream.RawAudioBlocks = \
	    build/operator/AudioStream/RawAudioBlocks.o \
	    build/type/BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An.o \
	    build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o 

bin/AudioStream.RawAudioBlocks.so: \
	$(SOAudioStream.RawAudioBlocks) \
	build/dep/AudioStream.RawAudioBlocks.so \
	build/ldOptions
	@echo ' [LD-so] so AudioStream.RawAudioBlocks'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AudioStream.RawAudioBlocks.so -fPIC -shared $(SOAudioStream.RawAudioBlocks) -lstreams-stdtk-runtime $(SPL_LDFLAGS)

SOAudioStream.AudioStream = \
	    build/operator/AudioStream/AudioStream.o \
	    build/type/Bk0Uw25SjlXvT1SWjTbdPsq3rYHs_0MyINGepUqpl6M4N_13fsN9z_008LvigF23BryMh_0YBpGEGMzYN6naxAG2ZCd.o \
	    build/type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.o 

bin/AudioStream.AudioStream.so: \
	$(SOAudioStream.AudioStream) \
	build/dep/AudioStream.AudioStream.so \
	build/ldOptions
	@echo ' [LD-so] so AudioStream.AudioStream'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/AudioStream.AudioStream.so -fPIC -shared $(SOAudioStream.AudioStream) $(SPL_LDFLAGS)

standalone: bin/standalone.exe

build/standalone/standalone.o: \
	    src/standalone/standalone.h \
	    src/standalone/standalone.cpp  \
	    build/cppOptions \
	    | build/standalone
	@echo ' [CXX-standalone] standalone'
	@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/standalone/standalone.cpp

bin/standalone.exe: build/standalone/standalone.o build/ldOptions
	@echo ' [LD-standalone] standalone'
	@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/standalone.exe build/standalone/standalone.o $(SO_STAND_LIB)
	@echo ' [LN-standalone] standalone '
	@ln -sf standalone bin/NeMoFileTranscription
build/standalone:
	-@mkdir -m 755 build/standalone

sablink:

	@ln -sf ../NeMoFileTranscription.sab bin/sab

bundle: NeMoFileTranscription.sab

-include build/dep/NeMoFileTranscription.sab.d
NeMoFileTranscription.sab: $(SPL_SO_BINS) $(SPL_ADL_FILE) build/dep/NeMoFileTranscription.sab.d bin/standalone.exe
	@echo ' [Bundle] NeMoFileTranscription.sab'
	@$(SPL_BUNDLE_BUILDER) $(SPL_OUTPUT_DIR_ROOT) $(SPL_OUTPUT_DIR) $(SPL_ADL_FILE) NeMoFileTranscription.sab

