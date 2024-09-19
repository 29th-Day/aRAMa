# What is aRAMa?

This is an TCPGecko rewrite and port for Aroma. It contains several improvements, including notifications, enabling and disabling specific features via the plugin config menu, and more.

### **At the time of writing this, this is a work in progress ~~and is not usable yet~~**.

# Disclaimer

Not all TCPGecko software is fully compatible with aRAMa out-of-the-box, e.g. some memory addresses may differ due to how Aroma plugins work. **Do not report unmodified software or codes as bugs** unless you're certain the issue is caused by aRAMa itself. For help or to report problems with specific software, feel free to join our [Discord Servers](#discord) or contact the respective software maintainers.

# Usage

The aRAMa config menu can be accessed via the Plugin Menu by pressing `L + Minus + D-Down`.

In the config menu are several options:

* `aRAMa active`: Activates/deactivates the entire plugin.

<!-- * `Notifications`: When enabled, you will get Aroma notifications when the plugin does various things, such as load an assembly code or encounter an error. -->

<!-- * `Save sent codes`: When a code is set from a client, such as JGeckoU, automatically save it to the SD card for offline use. -->

<!-- * `TCP Gecko`: Enable TCPGecko. -->

<!-- * `Code handler`: Enable the environment used for running assembly code. -->

<!-- * `Caffiine`: Enable Caffiine.  -->

<!-- * `Saviine`: Enable Saviine. -->

# Installing

1. Download `aRAMa.wps` from [Releases](#link-to-releases).
2. Copy `aRAMa.wps` into the `sd:/wiiu/environments/aroma/plugins` folder[^1].
3. Restart the console.

Make sure you have the following modules installed on your SD card.
* [WUMSLoader](https://github.com/wiiu-env/WUMSLoader)
* [FunctionPatcherModule](https://github.com/wiiu-env/FunctionPatcherModule)
* [MemoryMappingModule](https://github.com/wiiu-env/MemoryMappingModule)
* [NotificationModule](https://github.com/wiiu-env/NotificationModule)
* [KernelModule](https://github.com/wiiu-env/KernelModule)

# Compiling

If you want to compile aRAMa yourself, follow the next instructions. Clone and move into the directory.

```bash
git clone ...
cd aRAMa
```

## Docker
    
A Dockerfile is provided to compile aRAMa without any local installations. This is the recommended way; no need for managing installs and versions. Just have [Docker](https://www.docker.com/) installed and run

```bash
docker build -t wups/arama -o build .
```

`aRAMa.wps` should now be in `./build`.

## Local

To compile aRAMa you'll need:
* cmake
* [devkitPro](https://devkitpro.org/wiki/Getting_Started)
* [wut](https://github.com/devkitPro/wut)
* [WiiUPluginSystem](https://github.com/wiiu-env/WiiUPluginSystem)
* [libkernel](https://github.com/wiiu-env/libkernel)
* [libnotifications](https://github.com/wiiu-env/libnotifications)

After you installed the dependencies (recommended in order) run

```bash
cmake -S . -B build
cmake --build build
```

`aRAMa.wps` should now be in `./build`.

<!--
# Feature re-implementation checklist:

Though I tried, I cannot guarantee that everything works, because of what was necessarily changed in porting TCPGecko from the old libraries to the new equivalents. 

* Things related to kernel access may be depreciated and/or redundant to their non-kernel counterparts in the future.
* **Old codes are anticipated to not work due to RAM offsets, so please do not report unmodified old codes not working as an issue.**
* Assembly related commands will not do anything if the `Code handler` is disabled.

I am reliant on people testing it and getting back to me on the status of various features. A checklist of what has been reported to work is below. 

Commands:

- [x] Write 8 bit value
- [x] Write 16 bit value
- [x] Write 32 bit value
- [x] Read memory
- [x] Read memory with kernel access
- [x] Validate address range
- [ ] Disassemble range (currently disabled)
- [?] Disassemble memory
- [ ] Read compressed memory
- [x] Write with kernel access
- [x] Read with kernel access 
- [ ] Take a screenshot
- [x] Upload memory
- [x] Get the size of data buffer
- [x] Read a file
- [x] Read a directory
- [x] Replace a file
- [ ] "IOSU read file" (currently disabled)
- [x] Get version hash
- [x] Get code handler address
- [-] Read threads
- [x] Get account identifier
- [ ] Write screen (currently disabled)
- [?] Follow pointer
- [x] Get server status
- [x] Remote procedure call
- [x] Get symbol
- [x] Search memory 32
- [x] Advanced memory search
- [?] Execute assembly
- [ ] Pause the console
- [ ] Unpause the console
- [ ] See whether the console is paused or not
- [x] Get server version
- [-] Get OS version 
- [ ] Set data breakpoint
- [ ] Set instruction breakpoint
- [ ] Toggle breakpoint
- [ ] Remove all breakpoints
- [ ] Get stack trace
- [ ] Poke registers
- [ ] Get entry point address
- [ ] Run kernel copy service
- [ ] Persist assembly
- [ ] Clear assembly
-->

# Credits

* **c08oprkiua** for starting development on aRAMa
* **29th-Day** for contributing to aRAMa
* **Maschell** for help on Discord
* **lulsec2** (Discord) for coming up with the name "aRAMa"
* (The devs of) **FTPiiU** for implementation referencing
* **[BullyWiiPlaza & contributors](https://github.com/BullyWiiPlaza/tcpgecko?tab=readme-ov-file#credits)** for the Tiramisu/original version of TCPGecko
* **Quarky** for arGecko

# Help, inquiries, etc.

The following resources should provide the needed help if you encounter any problems. When contacting community members, remember that they are not obliged to repond and **stay friendly**!

## Discord

* [JGeckoU & TCPGecko](https://discord.com/invite/rSRM3RWDq4)
* [Aroma](https://discord.com/invite/bZ2rep2)
* [aRAMa](https://discord.com/invite/VPHr56hnbQ)

[^1]: You may wanna use the [FPTiiU Plugin](https://github.com/wiiu-env/ftpiiu_plugin)

<!-- Love over fear -->
