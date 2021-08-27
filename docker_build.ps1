param(
    [Parameter(Mandatory=$True, Position = 0)]
    [String] $target,
    [Parameter(Mandatory=$True, Position = 1)]
    [String] $buildType,
    [Parameter(Mandatory=$True, Position = 2)]
    [String] $containerRuntime
)

docker build `
    --target "$target" `
    -t "mwa_time_data_reconstructor/$target" `
    --build-arg "BUILD_TYPE=$buildType" `
    --build-arg "CONTAINER_RUNTIME=$containerRuntime" `
    --progress plain `
    .
