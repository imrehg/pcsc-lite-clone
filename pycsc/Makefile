# give some default values
PREFIX ?= /usr
DESTDIR ?= /
ifeq (, $(PYTHONVER))
	PYTHONVER=2.4
endif
PYTHON=python$(PYTHONVER)

build: pycsc.c
	$(PYTHON) setup.py build
	cp `find build -name PyCSC`/pycsc.so PyCSC/

install: build
	$(PYTHON) setup.py install --prefix=$(PREFIX) --root=$(DESTDIR)

clean:
	$(PYTHON) setup.py clean
	rm -rf build
	rm PyCSC/pycsc.so

doc: build
	rm -rf html
	PYTHONPATH=`find build -name PyCSC` epydoc PyCSC

