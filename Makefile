CXX=mpic++
ODIR=./obj
SRCDIR=./src
INCDIR=./src

# LIBS =-lgmpxx -lgmp
CXX_FLAGS =-Wc++11-extensions

_CONVOLVE_TEST_OBJ = Array2d.o convolve.o convolve_test.o PPM.o

CONVOLVE_TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_CONVOLVE_TEST_OBJ))

obj:
	mkdir -p $@

$(ODIR)/%.o: $(SRCDIR)/%.cpp | obj
	$(CXX) -std=c++11 -I$(INCDIR) -c -o $@ $<

convolve_test: $(CONVOLVE_TEST_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o convolve_test
