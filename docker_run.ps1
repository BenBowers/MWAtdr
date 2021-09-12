param(
    [Parameter(Mandatory=$True, Position = 0)]
    [String] $target,

    [Parameter(Mandatory=$False, Position = 1, ValueFromRemainingArguments=$true)]
    [String[]] $arguments
)

docker run -t "mwatdr/$target" @arguments
