include srcpathconfig.mk
include compileconfig.mk

tmpbuildtm := `date |sed 's/ /_/g'`
TMPBUILDTM = $(tmpbuildtm)

srcfile := $(foreach d,$(SRCCODEDIRS),$(wildcard $(addprefix $(d)/*,.c)))
objfile := $(patsubst %.c,%.o,$(srcfile))
depfile := $(patsubst %.c,%.d,$(srcfile))

curcmplmacro   := $(addprefix -D ,$(COMPILEMACRO))
cursrcheaddirs := $(addprefix -I ,$(SRCHEADDIRS))
curlibheaddirs := $(addprefix -I ,$(LIBHEADDIRS))

showgcctype:
	@echo ""
	@echo "now using " $(compiler) "compiler"
	
TARGETNAME = xc_app
$(TARGETNAME): showgcctype $(objfile)
	@echo ""
	@echo "all files have been compiled , now begin to link every obj for excutable file"
	@echo ""
	@echo "linking............"
	@echo $(objfile)
	@$(compiler) -DPBUILTTIME=\"$(TMPBUILDTM)\" -DPBUILDVERSION=\"$(BUILDVERSION)\" $(curcmplmacro) $(COMPILESWITCH) -o $(TARGETNAME) $(objfile) -L$(LIBFILEDIRS) $(LIBFILE)
	@echo ""
	@echo "linked ok," $(TARGETNAME) "has been created"
	@echo ""
	@echo $(TMPBUILDTM) $(BUILDVERSION)
%.o: %.c
	@echo ""
	@echo "start " $< "......compiling"
	@$(compiler) -DPBUILTTIME=\"$(TMPBUILDTM)\" -DPBUILDVERSION=\"$(BUILDVERSION)\" $(curcmplmacro) $(COMPILESWITCH) -c $(cursrcheaddirs) $(curlibheaddirs) $< -o $@
	@echo "created " $@
	@echo "end   " $< "......compiled ok"

.PHONY: arm linux clean

arm: $(TARGETNAME)
	
linux: $(TARGETNAME)

clean:
	@-$(RM) $(TARGETNAME) $(objfile) $(depfile)
