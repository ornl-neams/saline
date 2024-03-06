if ( !(Test-Path $CI_PROJECT_DIR/tools/micromamba.exe -PathType Leaf) )
{
    # Didn't find it so install micromamba
    mkdir -p $CI_PROJECT_DIR/tools
    cd $CI_PROJECT_DIR/tools

    Invoke-Webrequest -URI https://micro.mamba.pm/api/micromamba/win-64/latest -OutFile micromamba.tar.bz2
    $7z = "& '$env:PROGRAMFILES\7-zip\7z.exe'"
    $params = "-y x micromamba.tar.bz2"
    Invoke-Expression "$7z $params"
    $params = "-y x micromamba.tar"
    Invoke-Expression "$7z $params"
    MOVE -Force Library\bin\micromamba.exe micromamba.exe
    cd $CI_PROJECT_DIR
}

Invoke-Expression "$CI_PROJECT_DIR\tools\micromamba.exe shell hook -s powershell" | Out-String | Invoke-Expression
if (micromamba info | Select-String "saline_build" )
{
    echo "env already exists"
}
else
{
    micromamba create -f $CI_PROJECT_DIR/ci/build_env.yml -y
}
