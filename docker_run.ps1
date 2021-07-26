param(
    [String]$target
)

if ([string]::IsNullOrEmpty($target)) {
    Write-Output "Usage: docker_run.ps1 <target>"
    exit 1
}

docker run -t "mwa_time_data_reconstructor/$target"
