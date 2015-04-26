CXX=g++
ODIR=./obj
SRCDIR=./src
INCDIR=./src

# LIBS =-lgmpxx -lgmp
CXX_FLAGS =-Wc++11-extensions
CXX_ARGS=-std=c++11 -Wall -fopenmp

_CONVOLVE_TEST_OBJ = Array2d.o convolve.o convolve_test.o PPM.o

CONVOLVE_TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_CONVOLVE_TEST_OBJ))

obj:
	mkdir -p $@

$(ODIR)/%.o: $(SRCDIR)/%.cpp | obj
	$(CXX) $(CXX_ARGS) -I$(INCDIR) -c -o $@ $<

convolve_test: $(CONVOLVE_TEST_OBJ)
	$(CXX) $(CXX_ARGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o convolve_test
