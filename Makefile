PLUGIN_NAME=hypreasymotion
INSTALL_LOCATION=/usr/lib/
SOURCE_FILES=$(wildcard ./*.cpp)

all: $(PLUGIN_NAME).so

install: all
	mkdir -p ${INSTALL_LOCATION}
	sudo cp $(PLUGIN_NAME).so ${INSTALL_LOCATION}

$(PLUGIN_NAME).so: $(SOURCE_FILES)
	g++ -g -shared -Wall -fPIC --no-gnu-unique $(SOURCE_FILES) -g  -DWLR_USE_UNSTABLE `pkg-config --cflags pixman-1 libdrm hyprland pangocairo` -std=c++23 -o $(PLUGIN_NAME).so

clean:
	rm -f ./$(PLUGIN_NAME).so
