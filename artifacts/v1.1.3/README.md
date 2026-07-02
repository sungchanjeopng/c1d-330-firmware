# C1D-330 v1.1.3 firmware artifacts

Generated from GitHub repo `sungchanjeopng/c1d-330-firmware` after syncing the local v1.1.3 firmware/app changes.

## Files

- `c1d-330_bootload.bin`: bootloader binary from local bootloader branch.
- `c1d-330_v1.1.3_app.bin`: application binary built from this repository with `Makefile.hermes`.
- `c1d-330_v1.1.3_with_bootload.bin`: combined bootloader + app image for flashing at `0x08000000`.
- `c1d-330_v1.1.3_build.log`: build log from the verification build.
- `SHA256SUMS.txt`: SHA256 hashes.

## Build command

```sh
make -f Makefile.hermes BUILD_DIR=build_hermes_verify -j4 all
```

Toolchain:

```text
GNU Make 4.2.1_st_20200221-0903_longpath
arm-none-eabi-gcc 7.3.1 20180622 (GNU Tools for STM32 7-2018-q2-update)
```

Build result:

```text
text=464216, data=1576, bss=445736, dec=911528, hex=de8a8
warnings=75
errors=0
```

## Merge layout

```text
boot_origin = 0x08000000
app_origin  = 0x08008000
app_offset  = 0x8000 (32768 bytes)
boot_size   = 10616 bytes
gap_fill    = 22152 bytes of 0xFF
app_size    = 465800 bytes
merged_size = 498568 bytes
end_exclusive = 0x08079B88
```

Vector table check:

```text
boot_vector_sp    = 0x20080000
boot_vector_reset = 0x08000AE9
app_vector_sp     = 0x20080000
app_vector_reset  = 0x08041C45
```

Verification:

```text
boot_prefix=True
gap_all_ff=True
app_at_offset=True
boot_sp_ram_range=True
app_sp_ram_range=True
boot_reset_in_boot_region=True
app_reset_in_flash_after_app_origin=True
```

SHA256:

```text
642d6014f72495aa45051dd16fa0ccd18ea57d977436cdc052bd38b926f70c1c  c1d-330_bootload.bin
b698cf0635f01b7711e4b7330c7310986c41b6edf9c8a82e87642655efe528ef  c1d-330_v1.1.3_app.bin
8ebe1a2503a470c3f4926b807729ae3285dac1481b983801429da64e36f33314  c1d-330_v1.1.3_with_bootload.bin
e6fbb7565481267947291c7b9a2f5ce4ea679fe03d1ee8a0eef6a9cd441b9d8b  c1d-330_v1.1.3_build.log
```
