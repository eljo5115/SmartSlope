let port;
let writer;
let reader;

let isSetup = false;
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

function sendSetupCommand(){
    if (!writer) {
        alert("Connect to Arduino first!");
        return;
    }
    sendCommand("setup");
    isSetup = true;
}

async function sendCommand(command) 
{
    if (!writer) {
        alert("Connect to Arduino first!");
        return;
    }
    console.log("Sending...",command);
    const data = new TextEncoder().encode(command + "\n");
    await writer.write(data);
    if(command == "setup"){
        isSetup = true;
        document.getElementById('setup-button').hidden = true;
        document.getElementById('remove-button').hidden = false;
    }
    else if(command == "remove"){
        isSetup = false
        document.getElementById('setup-button').hidden = false;
        document.getElementById('remove-button').hidden = true;
    }
    
}

async function readSerialData() 
{

    /*
    -1 - removing
    0 - setup
    1 - left-right
    2 - Wonky Waves
    3 - right-left
    */
    while (port.readable) {
        try {
            const { value, done } = await reader.read();
            if (done) {
                console.log("Serial reading stopped.");
                reader.releaseLock();
                break;
            }

            let sRead = new TextDecoder().decode(value); // read from Serial port
            console.log("Raw serial:",sRead);
            let text = setText(Number(sRead));
            console.log("Arduino says:", text);
            document.getElementById("status").innerText = "Current setting " + text.trim();
        } catch (error) {
            console.error("Error reading serial data:", error);
        }
    }
}


function setText(s){
    console.log("SetText: ", s);
    switch(s){
        case -1:
            return "Removing Plates";
        case 0:
            return "Setup";
        case 1:
            return "Left-Right Break";
        case 2:
            return "Wonky Waves";
        case 3:
            return "Right-Left Break";
        case 4:
            return "Flat";
        case 5:
            return "2 tier";
        case 6:
            return "Tiger 2008 US Open";
        case 7:
            return "Challenge Mode";
        case 8:
            return "5 tier";
        case 9:
            return "6 tier";
        case 10:
            return "7 tier";
        case 11:
            return "8 tier";
        default:
            return "Idk probably broken command signal";
    }
}