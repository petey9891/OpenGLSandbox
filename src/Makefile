CXXFLAGS=-O2 -W -Wall -Wextra -Wno-unused-parameter -D_FILE_OFFSET_BITS=64

OBJECTS=app.o IndexBuffer.o Renderer.o Shader.o VertexArray.o VertexBuffer.o
BINARIES=app            

CURRENT_DIRECTORY=$(shell pwd)

# Where our library resides. You mostly only need to change the                               
# # RGB_LIB_DISTRIBUTION, this is where the library is checked out.                             
RGB_LIB_DISTRIBUTION=../../rpi-rgb-led-matrix
RGB_INCDIR=$(RGB_LIB_DISTRIBUTION)/include
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread -lbrcmEGL -lbrcmGLESv2 -lGLESv2 -I/opt/vc/include -L/opt/vc/lib

# -L$(CURRENT_DIRECTORY)/../lib
# -ldl -lX11 -lGL -lGLU -lGLEW -lbrcmEGL -lbrcmGLESv2 -lrt -lm -lpthread -I/opt/vc/include -L/opt/vc/lib

all : $(BINARIES)

$(RGB_LIBRARY): FORCE
	$(MAKE) -C $(RGB_LIBDIR)

# app: app.o $(RGB_LIBRARY)
app: $(OBJECTS) $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

%.o : %.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

app.o: application.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

IndexBuffer.o: IndexBuffer.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

Renderer.o: Renderer.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

Shader.o: Shader.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

VertexArray.o: VertexArray.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

VertexBuffer.o: VertexBuffer.cpp
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(BINARIES)

FORCE:
.PHONY: FORCE

# -I$(CURRENT_DIRECTORY)/../include
