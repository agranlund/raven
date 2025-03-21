
BOARD		= a1
DATE		= $(shell date +%Y%m%d)

FW_NESSI	= nessi_240901.jed
FW_NARNIA	= narnia_240422.jed
FW_IKSI		= iksi_240418.jed
FW_EIFFEL	= eiffel_1.10.2-raven_A1.hex
FW_SIMMPROG	= ravenprog.uf2

RELEASE_TAG	= Raven.A1.latest
RELEASE_DESC	= Raven.A1.$(DATE)
RELEASE_ZIP	= raven_a1_$(DATE).zip

ATARIDIR	= /Volumes/SHARE/asus/raven
ROOTDIR		= ..
DISTDIR		= $(ROOTDIR)/dist
SUPPDIR		= $(DISTDIR)/3rdparty
OUTDIR		= $(DISTDIR)/raven
HWDIR		= $(OUTDIR)/hardware
FWDIR		= $(OUTDIR)/firmware
ROMDIR		= $(OUTDIR)/roms
SRECDIR		= $(ROMDIR)/srec
SWDIR		= $(OUTDIR)/software
COPY		= cp -p


SWDIR_AUTO		= $(SWDIR)/auto
SWDIR_CPX		= $(SWDIR)/cpx
SWDIR_NOVA		= $(SWDIR)/nova
SWDIR_TOOLS		= $(SWDIR)/tools
SWDIR_RTOOLS	= $(SWDIR)/raven
SWDIR_ISA		= $(SWDIR)/raven/drivers


.phony: release release-test
.phony: setup clean copy upload
.phony: copydocs copyroms copyfw

release: setup copy zipit upload

release-test: setup copy zipit

setup: clean
	mkdir -p $(OUTDIR)

clean:
	rm -r -f $(OUTDIR)
	rm -f *.zip	

copy: copydocs copyhw copyfw copyroms copysw

copydocs:
	$(COPY) $(ROOTDIR)/doc/raven_a1.pdf $(OUTDIR)/manual.pdf
	$(COPY) $(DISTDIR)/license.gpl $(OUTDIR)/license.gpl

copyroms:
	mkdir -p $(ROMDIR)
	$(COPY) $(DISTDIR)/readme_rom.txt $(ROMDIR)/readme.txt
	make -C $(ROOTDIR)/sw/rom clean
	make -C $(ROOTDIR)/sw/rom tos-dbg
	$(COPY) $(ROOTDIR)/sw/rom/rom_dbg.bin $(ROMDIR)/
	make -C $(ROOTDIR)/sw/rom clean
	make -C $(ROOTDIR)/sw/rom tos
	$(COPY) $(ROOTDIR)/sw/rom/rom_tos.bin $(ROMDIR)/
	make -C $(ROOTDIR)/sw/rom mon
	$(COPY) $(ROOTDIR)/sw/rom/rom_mon.bin $(ROMDIR)/
	mkdir -p $(SRECDIR)
	make -C $(ROOTDIR)/sw/rom/srec clean
	make -C $(ROOTDIR)/sw/rom/srec all
	$(COPY) $(ROOTDIR)/sw/rom/srec/selftest/selftest.s19 $(SRECDIR)/

copyfw:
	mkdir -p $(FWDIR)
	$(COPY) $(DISTDIR)/readme_fw.txt $(FWDIR)/readme.txt
	$(COPY) $(ROOTDIR)/fw/$(BOARD)/Nessi/$(FW_NESSI) $(FWDIR)/
	$(COPY) $(ROOTDIR)/fw/$(BOARD)/Narnia/$(FW_NARNIA) $(FWDIR)/
	$(COPY) $(ROOTDIR)/fw/$(BOARD)/Iksi/$(FW_IKSI) $(FWDIR)/
	make -C $(ROOTDIR)/fw/eiffel
	$(COPY) $(ROOTDIR)/fw/eiffel/$(FW_EIFFEL) $(FWDIR)/eiffel.hex
	$(COPY) $(ROOTDIR)/hw/simm/programmer/firmware/build/$(FW_SIMMPROG) $(FWDIR)/


copyhw:
	mkdir -p $(HWDIR)
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_gerber.zip $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_bom.md $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_ibom.html $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_gerber.zip $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_sch.pdf $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_brd.pdf $(HWDIR)/

zipit:
	find raven -name ".DS_Store" -delete
	zip -r -o $(RELEASE_ZIP) raven/*

upload: github_delete_release github_create_release



.phony: github_delete_release
github_delete_release:
	gh release delete $(RELEASE_TAG) --cleanup-tag -y

.phony: github_create_release
github_create_release:
	gh release create $(RELEASE_TAG) --notes-file "release_notes.txt"
	gh release upload $(RELEASE_TAG) $(RELEASE_ZIP) --clobber


copysw:
	mkdir -p $(SWDIR)
	mkdir -p $(SWDIR_AUTO)
	mkdir -p $(SWDIR_CPX)
	mkdir -p $(SWDIR_TOOLS)
	mkdir -p $(SWDIR_RTOOLS)
	mkdir -p $(SWDIR_ISA)

	# --------------------------------------------------
	# General stuff
	# --------------------------------------------------
	$(COPY) $(DISTDIR)/readme_sw.txt $(SWDIR)/raven.txt


	# --------------------------------------------------
	# RVBIOS
	# --------------------------------------------------
	make -C $(ROOTDIR)/sw/rvbios clean
	make -C $(ROOTDIR)/sw/rvbios
	$(COPY) $(ROOTDIR)/sw/rvbios/rvbios.prg $(SWDIR_AUTO)/


	# --------------------------------------------------
	# RVNOVA
	# --------------------------------------------------
	make -C $(ROOTDIR)/sw/rvnova clean
	make -C $(ROOTDIR)/sw/rvnova
	$(COPY) $(ROOTDIR)/sw/rvnova/rvnova.prg $(SWDIR_AUTO)/

	# --------------------------------------------------
	# TOOLS
	# --------------------------------------------------
	make -C $(ROOTDIR)/sw/tools clean
	make -C $(ROOTDIR)/sw/tools
	$(COPY) $(ROOTDIR)/sw/tools/thingpal/thingpal.tos $(SWDIR_TOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/mon/mon.tos $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/cacheon/cacheon.prg $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/cacheoff/cacheoff.prg $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/resetnvm/resetnvm.prg $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/patch_xc/patch_xc.prg $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/fpu_emu/fpemu.prg $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/fpu_emu/fpemu.txt $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/flash/flash.ttp $(SWDIR_RTOOLS)/
	$(COPY) $(ROOTDIR)/sw/tools/serial.cpx/serial.cpx $(SWDIR_CPX)/

	# --------------------------------------------------
	# ISA BIOS
	# --------------------------------------------------
	make -C $(ROOTDIR)/sw/isa/isa_bios clean
	make -C $(ROOTDIR)/sw/isa/isa_bios
	$(COPY) $(ROOTDIR)/sw/isa/isa_bios/isa_bios.prg $(SWDIR_AUTO)/
	$(COPY) $(ROOTDIR)/sw/isa/isa_bios/isa_bios.inf $(SWDIR)/


	# --------------------------------------------------
	# ISA DRIVERS
	# --------------------------------------------------

		# --------------------------------------------------
		# FMRADIO
		# --------------------------------------------------
		mkdir -p $(SWDIR_ISA)/fmradio
		make -C $(ROOTDIR)/sw/isa/drivers/fmradio clean
		make -C $(ROOTDIR)/sw/isa/drivers/fmradio
		$(COPY) $(ROOTDIR)/sw/isa/drivers/fmradio/fmradio.prg $(SWDIR_ISA)/fmradio/fmradio.prg
		$(COPY) $(ROOTDIR)/sw/isa/drivers/fmradio/fmradio.txt $(SWDIR_ISA)/fmradio/fmradio.txt

		# --------------------------------------------------
		# MPU401
		# --------------------------------------------------
		mkdir -p $(SWDIR_ISA)/mpu401
		make -C $(ROOTDIR)/sw/isa/drivers/mpu401 clean
		make -C $(ROOTDIR)/sw/isa/drivers/mpu401
		$(COPY) $(ROOTDIR)/sw/isa/drivers/mpu401/mpu401.prg $(SWDIR_ISA)/mpu401/mpu401.prg
		$(COPY) $(ROOTDIR)/sw/isa/drivers/mpu401/mpu401.txt $(SWDIR_ISA)/mpu401/mpu401.txt

		# --------------------------------------------------
		# OPL-MIDI
		# --------------------------------------------------
		mkdir -p $(SWDIR_ISA)/oplmidi
		make -C $(ROOTDIR)/sw/isa/drivers/oplmidi clean
		make -C $(ROOTDIR)/sw/isa/drivers/oplmidi
		$(COPY) $(ROOTDIR)/sw/isa/drivers/oplmidi/oplmidi.prg $(SWDIR_ISA)/oplmidi/oplmidi.prg
		$(COPY) $(ROOTDIR)/sw/isa/drivers/oplmidi/oplmidi.txt $(SWDIR_ISA)/oplmidi/oplmidi.txt

		# --------------------------------------------------
		# GUS
		# --------------------------------------------------
		mkdir -p $(SWDIR_ISA)/gus
		make -C $(ROOTDIR)/sw/isa/drivers/gus clean
		make -C $(ROOTDIR)/sw/isa/drivers/gus
		$(COPY) $(ROOTDIR)/sw/isa/drivers/gus/ultrinit.prg $(SWDIR_ISA)/gus/ultrinit.prg

		# --------------------------------------------------
		# PicoGUS
		# --------------------------------------------------
		mkdir -p $(SWDIR_ISA)/picogus
		make -C $(ROOTDIR)/sw/isa/drivers/picogus/pgusinit -f Makefile-atari clean
		make -C $(ROOTDIR)/sw/isa/drivers/picogus/pgusinit -f Makefile-atari
		$(COPY) $(ROOTDIR)/sw/isa/drivers/picogus/pgusinit/pgusinit.prg $(SWDIR_ISA)/picogus/pgusinit.prg
		$(COPY) $(ROOTDIR)/sw/isa/drivers/picogus/pgusinit/README.md $(SWDIR_ISA)/picogus/readme.txt

		# --------------------------------------------------
		# NE2000
		# --------------------------------------------------
		mkdir -p $(SWDIR_ISA)/ne2000
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/ENEH.MIF $(SWDIR_ISA)/ne2000/eneh.xif
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/ENEH.STX $(SWDIR_ISA)/ne2000/eneh.stx
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/ETHERNE.TXT $(SWDIR_ISA)/ne2000/etherne.txt
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/COPYING.TXT $(SWDIR_ISA)/ne2000/copying.txt

		make -C $(ROOTDIR)/sw/tools/uiptools
		$(COPY) $(ROOTDIR)/sw/tools/uiptools/uip.tos $(SWDIR_ISA)/ne2000/uip.tos
		$(COPY) $(ROOTDIR)/sw/tools/uiptools/uip.txt $(SWDIR_ISA)/ne2000/uip.txt


	# --------------------------------------------------
	# Castaway
	# --------------------------------------------------
		mkdir -p $(SWDIR_RTOOLS)/castaway
		# todo: clean and recompile?
		$(COPY) $(ROOTDIR)/../castaway/bin/* $(SWDIR_RTOOLS)/castaway/
		$(COPY) $(ROOTDIR)/../castaway/readme.txt $(SWDIR_RTOOLS)/castaway/
		$(COPY) $(ROOTDIR)/../castaway/license.txt $(SWDIR_RTOOLS)/castaway/


	# --------------------------------------------------
	# Eiffel
	# --------------------------------------------------
		mkdir -p $(SWDIR_RTOOLS)/eiffel
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/eiffelcf.app" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/copying" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/eiffel_g.rsc" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/doc/eiffel.hyp" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/doc/eiffel.ref" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/doc/authors.txt" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/doc/Technik.txt" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/french/*" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/german/*" -d $(SWDIR_RTOOLS)
		unzip -L $(SUPPDIR)/eiffel/eiffel1110.zip "eiffel/tab_keyb/*" -d $(SWDIR_RTOOLS)
		$(COPY) -R $(SUPPDIR)/eiffel/tab_keyb/* $(SWDIR_RTOOLS)/eiffel/tab_keyb/


	# --------------------------------------------------
	# NOVA
	# --------------------------------------------------
		mkdir -p $(SWDIR_NOVA)
		unzip $(SUPPDIR)/nova/TT_ET4000T0_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T0 $(SWDIR_NOVA)/ET4000.T0
		unzip $(SUPPDIR)/nova/TT_ET4000T3_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T3 $(SWDIR_NOVA)/ET4000.T3
		unzip $(SUPPDIR)/nova/TT_ET4000T4_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T4 $(SWDIR_NOVA)/ET4000.T4
		unzip $(SUPPDIR)/nova/TT_ET4000T8_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T8 $(SWDIR_NOVA)/ET4000.T8
		unzip $(SUPPDIR)/nova/TT_ET4000W32_V301.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/TT $(SWDIR_NOVA)/ET4000.W32
		unzip $(SUPPDIR)/nova/TT_Mach32_V301.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/TT $(SWDIR_NOVA)/MACH32
		unzip $(SUPPDIR)/nova/TT_Mach64_V303.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/M64 $(SWDIR_NOVA)/MACH64

		$(COPY) -r $(SUPPDIR)/nova.bib/* $(SWDIR_NOVA)/

		unzip -j $(SUPPDIR)/nova/NOVA_COL_171.zip "NOVA_COE.PRG" -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/NOVA_COE.PRG $(SWDIR)/NOVA_COL.ACC
		unzip -j $(SUPPDIR)/nova/vme8_0.zip "VME.8/CPX/*" -d $(SWDIR_NOVA)
		unzip -L $(SUPPDIR)/nova/XMENU_ENG.zip -d $(SWDIR_NOVA)
		unzip -j $(SUPPDIR)/nova/SAVEOFF.zip -d $(SWDIR_NOVA)/

		$(COPY) $(SUPPDIR)/nova/NOVA_COL.INF $(SWDIR)/
		$(COPY) $(SUPPDIR)/nova/STA_VDI.INF $(SWDIR_AUTO)/sta_vdi.inf


	# --------------------------------------------------
	# Additional support tools
	# --------------------------------------------------
		mkdir -p $(SWDIR_TOOLS)/autosort
		unzip -j -L $(SUPPDIR)/Autosort.zip "autosort/autosort.prg" -d $(SWDIR_TOOLS)/autosort/
		unzip -j -L $(SUPPDIR)/Autosort.zip "autosort/autosort.doc" -d $(SWDIR_TOOLS)/autosort/

		mkdir -p $(SWDIR_TOOLS)/autoexec
		unzip -j -L $(SUPPDIR)/autoexec-for-tos-main.zip "autoexec-for-tos-main/autoexec.prg" -d $(SWDIR_TOOLS)/autoexec/
		unzip -j -L $(SUPPDIR)/autoexec-for-tos-main.zip "autoexec-for-tos-main/readme.md" -d $(SWDIR_TOOLS)/autoexec/
		$(COPY) $(SUPPDIR)/autoexec.cnf $(SWDIR)/

		mkdir -p $(SWDIR_TOOLS)/setenv
		unzip -j -L $(SUPPDIR)/setenv-main.zip "setenv-main/setenv.prg" -d $(SWDIR_TOOLS)/setenv/
		unzip -j -L $(SUPPDIR)/setenv-main.zip "setenv-main/readme.md" -d $(SWDIR_TOOLS)/setenv/
		$(COPY) $(SUPPDIR)/setenv.txt $(SWDIR)/

		mkdir -p $(SWDIR_TOOLS)/setflags
		unzip -j -L $(SUPPDIR)/setfl200.zip -d $(SWDIR_TOOLS)/setflags/
		mv $(SWDIR_TOOLS)/setflags/README $(SWDIR_TOOLS)/setflags/readme
		mv $(SWDIR_TOOLS)/setflags/SETFLAGS.APP $(SWDIR_TOOLS)/setflags/setflags.app

		$(COPY) $(SWDIR_TOOLS)/autoexec/autoexec.prg $(SWDIR_AUTO)/
		$(COPY) $(SWDIR_TOOLS)/setenv/setenv.prg $(SWDIR_AUTO)/
