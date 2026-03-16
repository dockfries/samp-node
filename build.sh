#!/bin/bash
set -euo pipefail

# Validate input args
if [ $# -lt 1 ]; then
  echo "Usage: $0 <plugin_version>"
  echo "Example: $0 22.22.1"
  exit 1
fi

PLUGIN_VERSION=$1
PLATFORM_DIR="releases/linux"  # output dir

# Optional: validate version format
if ! [[ "$PLUGIN_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+(-.+)?$ ]]; then
  echo "Error: PLUGIN_VERSION '$PLUGIN_VERSION' is not a valid version (e.g. 22.22.1 or 22.22.1-beta)"
  exit 1
fi

echo "Building for: linux, Version: $PLUGIN_VERSION"

# Clean and prepare platform directory
rm -rf "$PLATFORM_DIR"
mkdir -p "$PLATFORM_DIR"

# Build Docker image
echo "Building Docker image..."
docker build -f Dockerfile.linux -t samp-node-build --build-arg PLUGIN_VERSION="$PLUGIN_VERSION" . || {
  echo "Docker build failed"
  exit 1
}

# Run container to produce output
echo "Running container to build plugin..."
docker run --rm -v "$(pwd)/$PLATFORM_DIR:/work/releases" samp-node-build || {
  echo "Docker run failed"
  exit 1
}

echo "Build successful. Output files in: $PLATFORM_DIR/"
ls -lh "$PLATFORM_DIR/" || echo "Could not list output directory contents."
