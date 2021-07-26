param(
    [String]$target
)

if ([string]::IsNullOrEmpty($target)) {
    Write-Output "Usage: docker_build.ps1 <target>"
    exit 1
}

docker build --target "$target" -t "mwa_time_data_reconstructor/$target" --build-arg DOCKER_BUILD=1 .
