FROM ubuntu:jammy
LABEL version="1.0"
LABEL description="prepares the ppa release using the make_release scripts"

ARG MCRL2_RELEASE_BRANCH="release-202206"
ARG MCRL2_VERSION="202206.0"

# 1. Install packages needed for compilation.
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
 build-essential \
 cmake \
 git \
 libboost-dev \
 libgl1-mesa-dev \
 qtbase5-dev \
 ubuntu-dev-tools \
 debhelper \
 python3-psutil \
 python3-yaml

# 2. Clone the latest version of the repository in case that the make_release script must be updated.
RUN mkdir ~/ppa && cd ~/ppa && git clone https://github.com/mcrl2org/mcrl2.git mcrl2

# 3. Build the mCRL2 release.
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION build_mcrl2_release"]

# 4. Package the source package.
RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION build_source_package"]

# Here we have to add the gpg key into the docker image.

# 5. Test the debian packages.
#RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package bionic "]
#RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package focal "]
#RUN ["/bin/bash", "-c", "cd ~/packaging && source mcrl2/build/make_release/make_release && MCRL2_VERSION=$MCRL2_VERSION test_ppa_package jammy "]

