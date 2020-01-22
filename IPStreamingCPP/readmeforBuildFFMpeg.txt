
#Start PowerShell into this folder
# BuildLib 0 = no build of Additional Libs
# Platforms =ARM build ARM Platform


./Build-FFmpeg.ps1 -BuildLib 0 -Platforms ARM

# Build all Platforms but without Libs
./Build-FFmpeg.ps1 -BuildLib 0 