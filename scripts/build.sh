echo "Regenerating global variables..."
python ./scripts/GenerateHeaders.py
echo "Running Docker..."
docker run --rm --user $(id -u):$(id -g) -v ${PWD}:/workspace touhou-builder python3 scripts/GenerateVfs.py /workspace/msvc_env
docker run --rm --user $(id -u):$(id -g) -v ${PWD}:/workspace touhou-builder
