param(
    [Parameter(Mandatory=$True, Position = 0)]
    [String] $target
)

docker build --target "$target" -t "mwa_time_data_reconstructor/$target" .
