# MKELibrary v2

hello. 

if you do not know how to use MKELibrary, read `doc/main.md`.

you are using MKELibrary v2. please update `config.txt` to match your system setup.

if you are trying to develop a new project, please copy the entire folder `template/generic` or `template/rtos` appropriately.

if you would like to use the old MKELibrary, `$ git checkout v1`.

## troubleshooting

### booting into rom

symptoms:

- chip doesn't appear to be running code without the debugger attached
- when connecting to a running program after restart, instruction is at 0x1C00xxxx

disease:

- NMI_b is pulled down during power-up

cure: 

- change NV_FOPT_BOOTPIN_OPT from 0 (default) to 1 by changing the flash config in the startup file:

old:

```
/* Flash Configuration */
    .section .FlashConfig, "a"
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFF7DFE
```

new:

```
/* Flash Configuration */
    .section .FlashConfig, "a"
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFF7FFE
```
