FROM rockylinux:10

RUN dnf update -y && dnf install -y cmake g++ make python3 && \
    python3 -m venv venv && . venv/bin/activate && \
    pip install --upgrade pip conan && \
    conan profile detect

COPY src ./src
COPY include ./include
COPY CMakeLists.txt conanfile.py ./

RUN . venv/bin/activate && \
    conan build && conan export && \
    conan cache save linxer --file linxer.tgz
