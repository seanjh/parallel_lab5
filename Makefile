CXX=g++
ODIR=./obj
SRCDIR=./src
INCDIR=./src

# LIBS =-lgmpxx -lgmp
# CXX_FLAGS =-Wc++11-extensions
CXX_FLAGS=-std=c++11 -Wall -fopenmp

_CONVOLVE_TEST_OBJ = Array2d.o convolve.o convolve_test.o PPM.o PNM.o Stencil.o
CONVOLVE_TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_CONVOLVE_TEST_OBJ))

_FILTER_IMAGE_OBJ = Array2d.o convolve.o filter_image.o PPM.o PNM.o Stencil.o
FILTER_IMAGE_OBJ = $(patsubst %,$(ODIR)/%,$(_FILTER_IMAGE_OBJ))

obj:
	mkdir -p $@

$(ODIR)/%.o: $(SRCDIR)/%.cpp | obj
	$(CXX) $(CXX_FLAGS) -I$(INCDIR) -c -o $@ $<

convolve_test: $(CONVOLVE_TEST_OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LIBS)

filter_image: $(FILTER_IMAGE_OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o convolve_test filter_image
