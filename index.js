let port;
let writer;
let reader;

async function connectSerial() 
{
    try {
        port = await navigator.serial.requestPort(); // Request access to the serial port
        await port.open({ baudRate: 9600 }); // Open port with 9600 baud

        // Setup writer for sending data
        writer = port.writable.getWriter();

        // Read incoming data
        reader = port.readable.getReader();
        readSerialData();
        document.getElementById("status").innerText = "Connected to Arduino!";
    } catch (err) {
        console.error("Serial connection error:", err);
        alert("Failed to connect to Arduino.");
    }
}

async function sendCommand(command) 
{
    if (!writer) {
        alert("Connect to Arduino first!");
        return;
    }
    const data = new TextEncoder().encode(command + "\n");
    await writer.write(data);
}

async function readSerialData() 
{
    while (port.readable) {
        try {
            const { value, done } = await reader.read();
            if (done) {
                console.log("Serial reading stopped.");
                reader.releaseLock();
                break;
            }
            var string = "";
            let text = new TextDecoder().decode(value);
            console.log("Arduino says:", text);
            document.getElementById("status").innerText = "LED Status: " + text.trim();
        } catch (error) {
            console.error("Error reading serial data:", error);
        }
    }
}