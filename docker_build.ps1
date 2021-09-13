param(
    [Parameter(Mandatory=$True, Position = 0)]
    [String] $target,
    [Parameter(Mandatory=$True, Position = 1)]
    [String] $buildType,
    [Parameter(Mandatory=$True, Position = 2)]
    [String] $runtimeSystem,
    [Parameter(Mandatory=$True, Position = 3)]
    [String] $containerRuntime
)

docker build `
    --target "$target" `
    -t "mwatdr/$target" `
    --build-arg "BUILD_TYPE=$buildType" `
    --build-arg "RUNTIME_SYSTEM=$runtimeSystem" `
    --build-arg "CONTAINER_RUNTIME=$containerRuntime" `
    --progress plain `
    .
