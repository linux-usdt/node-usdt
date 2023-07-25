const USDT = require("./usdt");

const provider = new USDT.USDTProvider("nodeProvider");
const probe1 = provider.addProbe("firstProbe", "int", "char *");
provider.enable();
let countdown = 10;

function waiter() {
  console.log("Firing probe...");
  probe1.fire(function() {
    console.log("Probe fired!");
    countdown = countdown - 1;
    return [countdown, "My little string"];
  });
}

setInterval(waiter, 3000);
