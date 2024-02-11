#!/bin/bash

# Look for micromamba install
if ! [ -f "$CI_PROJECT_DIR/../tools/bin/micromamba" ]; then
    # Didn't find it so install micromamba
    mkdir -p "$CI_PROJECT_DIR/../tools/"
    cd "$CI_PROJECT_DIR/../tools/"

    # TO be clear this is hardly adequate in general and only meets what I know
    # is going to happen
    if [ "$(uname)" == "Darwin" ]; then
        curl -Lks https://micro.mamba.pm/api/micromamba/osx-64/latest | tar -xvj bin/micromamba
    elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
        curl -Lks https://micro.mamba.pm/api/micromamba/linux-64/latest | tar -xvj bin/micromamba
    fi
    cd $CI_PROJECT_DIR
fi

# Get things set up to run mamba
eval "$($CI_PROJECT_DIR/../tools/bin/micromamba shell hook --shell bash --root-prefix $MAMBA_ROOT_PREFIX)"

# Ensure we have the environment built| grep -q base; then echo "base already exists"; fi
if micromamba info --envs | grep -q saline_build; then
    echo "env already exists"
else
    # Since it wasn't installed create the environment while we are here
    micromamba create -f $CI_PROJECT_DIR/ci/build_env.yml -y
fi
