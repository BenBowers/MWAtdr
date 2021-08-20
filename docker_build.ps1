param(
    [Parameter(Mandatory=$True, Position = 0)]
    [String] $target,
    [Parameter(Mandatory=$True, Position = 1)]
    [String] $buildType
)

docker build --target "$target" -t "mwa_time_data_reconstructor/$target" --build-arg "BUILD_TYPE=$buildType" .
