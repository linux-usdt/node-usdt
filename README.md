# node-usdt

N-API addon to create USDT probes and instrument your application easily and without
rebuilding Node.

# Dependencies

At the moment this module only runs on Linux and requires
[libstapsdt](https://github.com/linux-usdt/libstapsdt) to be installed to create
runtime probes.

## Ubuntu 16.04

To install libstapsdt, run:

```bash
sudo add-apt-repository ppa:sthima/oss
sudo apt-get update
sudo apt-get install libstapsdt0 libstapsdt-dev
```

## Other

Build from [libstapsdt](https://github.com/linux-usdt/libstapsdt).

# Install

```bash
npm install usdt
```

# Example

The following code will create a probe named `firstProbe`.

```javascript
const USDT = require("usdt");

const provider = new USDT.USDTProvider("nodeProvider");
const probe1 = provider.addProbe("firstProbe", "int", "char *");
provider.enable();
let countdown = 10;

function waiter() {
  console.log("Firing probe...");
  if(countdown <= 0) {
    console.log("Disable provider");
    provider.disable();
  }
  probe1.fire(function() {
    console.log("Probe fired!");
    countdown = countdown - 1;
    return [countdown, "My little string"];
  });
}

setInterval(waiter, 750);
```

You can then trace this probe with any tool able to trace Systemtap's probes.
Here's an example with eBPF/bcc (assuming node test.js):

```bash
sudo trace-bpfcc -p $(pgrep -f test.js) 'u::firstProbe "%d - %s", arg1, arg2'
```
