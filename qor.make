# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),debug)
  OBJDIR     = obj/Debug
  TARGETDIR  = bin
  TARGET     = $(TARGETDIR)/qor
  DEFINES   += -DDEBUG -D_DEBUG
  INCLUDES  += -I/usr/include/freetype2 -I/usr/local/include/physx -Ithird_party/include
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -fno-strict-aliasing -std=c++0x
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   +=  -Lthird_party/lib
  LIBS      += -lpthread -lGL -lGLU -lSDL -lSDLmain -lGLEW -lassimp -lIL -lILU -lopenal -lalut -lSimulationController -lSceneQuery -lLowLevel -lLowLevelCloth -lPhysX3 -lPhysX3Vehicle -lPhysX3Cooking -lPhysX3Extensions -lPhysX3CharacterKinematic -lRepX3 -lRepXUpgrader3 -lPhysXProfileSDK -lPhysXVisualDebuggerSDK -lPxTask -lPhysX3Common -logg -lvorbis -lvorbisfile -lftgl -lboost_system -lboost_filesystem -langelscript -lrt
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release)
  OBJDIR     = obj/Release
  TARGETDIR  = bin
  TARGET     = $(TARGETDIR)/qor
  DEFINES   += -DNDEBUG
  INCLUDES  += -I/usr/include/freetype2 -I/usr/local/include/physx -Ithird_party/include
  CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2 -fno-strict-aliasing -std=c++0x
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s  -Lthird_party/lib
  LIBS      += -lpthread -lGL -lGLU -lSDL -lSDLmain -lGLEW -lassimp -lIL -lILU -lopenal -lalut -lSimulationController -lSceneQuery -lLowLevel -lLowLevelCloth -lPhysX3 -lPhysX3Vehicle -lPhysX3Cooking -lPhysX3Extensions -lPhysX3CharacterKinematic -lRepX3 -lRepXUpgrader3 -lPhysXProfileSDK -lPhysXVisualDebuggerSDK -lPxTask -lPhysX3Common -logg -lvorbis -lvorbisfile -lftgl -lboost_system -lboost_filesystem -langelscript -lrt
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/Filesystem.o \
	$(OBJDIR)/NodeFactory.o \
	$(OBJDIR)/Scene.o \
	$(OBJDIR)/EnvironmentNode.o \
	$(OBJDIR)/Console.o \
	$(OBJDIR)/Mesh.o \
	$(OBJDIR)/Material.o \
	$(OBJDIR)/Main.o \
	$(OBJDIR)/DynamicTyping.o \
	$(OBJDIR)/Engine.o \
	$(OBJDIR)/DummyPartitioner.o \
	$(OBJDIR)/TrackerNode.o \
	$(OBJDIR)/Entity.o \
	$(OBJDIR)/Renderer.o \
	$(OBJDIR)/Frustum.o \
	$(OBJDIR)/Spectator.o \
	$(OBJDIR)/Freq.o \
	$(OBJDIR)/PropertyList.o \
	$(OBJDIR)/Physics.o \
	$(OBJDIR)/Node.o \
	$(OBJDIR)/Animation.o \
	$(OBJDIR)/Settings.o \
	$(OBJDIR)/ShadowBuffer.o \
	$(OBJDIR)/Developer.o \
	$(OBJDIR)/Graphics.o \
	$(OBJDIR)/Light.o \
	$(OBJDIR)/Log.o \
	$(OBJDIR)/Batch.o \
	$(OBJDIR)/ParticleSystem.o \
	$(OBJDIR)/Texture.o \
	$(OBJDIR)/Actor.o \
	$(OBJDIR)/Shader.o \
	$(OBJDIR)/Audio.o \
	$(OBJDIR)/IPhysicsObject.o \
	$(OBJDIR)/GUI.o \
	$(OBJDIR)/Indicator.o \
	$(OBJDIR)/Input.o \
	$(OBJDIR)/EulerNode.o \
	$(OBJDIR)/GameState.o \
	$(OBJDIR)/pn.o \
	$(OBJDIR)/pnUtil.o \
	$(OBJDIR)/pnSocket.o \
	$(OBJDIR)/pnBuffer.o \
	$(OBJDIR)/SelectorNode.o \
	$(OBJDIR)/EditorState.o \
	$(OBJDIR)/matrixstack.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking qor
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning qor
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	-$(SILENT) cp $< $(OBJDIR)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
endif

$(OBJDIR)/Filesystem.o: src/Filesystem.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/NodeFactory.o: src/NodeFactory.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Scene.o: src/Scene.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/EnvironmentNode.o: src/EnvironmentNode.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Console.o: src/Console.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Mesh.o: src/Mesh.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Material.o: src/Material.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Main.o: src/Main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DynamicTyping.o: src/DynamicTyping.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Engine.o: src/Engine.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/DummyPartitioner.o: src/DummyPartitioner.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/TrackerNode.o: src/TrackerNode.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Entity.o: src/Entity.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Renderer.o: src/Renderer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Frustum.o: src/Frustum.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Spectator.o: src/Spectator.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Freq.o: src/Freq.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/PropertyList.o: src/PropertyList.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Physics.o: src/Physics.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Node.o: src/Node.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Animation.o: src/Animation.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Settings.o: src/Settings.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/ShadowBuffer.o: src/ShadowBuffer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Developer.o: src/Developer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Graphics.o: src/Graphics.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Light.o: src/Light.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Log.o: src/Log.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Batch.o: src/Batch.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/ParticleSystem.o: src/ParticleSystem.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Texture.o: src/Texture.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Actor.o: src/Actor.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Shader.o: src/Shader.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Audio.o: src/Audio.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/IPhysicsObject.o: src/IPhysicsObject.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/GUI.o: src/GUI.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Indicator.o: src/Indicator.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/Input.o: src/Input.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/EulerNode.o: src/EulerNode.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/GameState.o: src/game/GameState.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/pn.o: src/pn/pn.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/pnUtil.o: src/pn/pnUtil.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/pnSocket.o: src/pn/pnSocket.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/pnBuffer.o: src/pn/pnBuffer.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/SelectorNode.o: src/editor/SelectorNode.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/EditorState.o: src/editor/EditorState.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"
$(OBJDIR)/matrixstack.o: src/math/matrixstack.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
