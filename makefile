.PHONY: default all build test docker-build save clean

VERSION?=0.1.1
DOCKER?=podman

defaut: all

all: build test docker-build save

build:
	conan build && conan export

test:
	conan test test_package linxer/0.1.0 --build missing

docker-build:
	$(DOCKER) build . -t wisdombeard/linxer:$(VERSION)

save:
	id=$$($(DOCKER) create localhost/wisdombeard/linxer:$(VERSION)) && \
    $(DOCKER) cp $$id:/linxer.tgz ./ && $(DOCKER) rm -v $$id

clean:
	rm -rf build && $(DOCKER) rmi -i wisdombeard/linxer:$(VERSION)
