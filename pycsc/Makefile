# give some default values
PREFIX ?= /usr
DESTDIR ?= /
ifeq (, $(PYTHONVER))
	PYTHONVER=2.4
endif
PYTHON=python$(PYTHONVER)

build:
	$(PYTHON) setup.py build

install: build
	$(PYTHON) setup.py install --prefix=$(PREFIX) --root=$(DESTDIR)

clean:
	$(PYTHON) setup.py clean
	rm -rf build

