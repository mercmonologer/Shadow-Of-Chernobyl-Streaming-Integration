const WebSocket = require("ws");
const net = require("net")

const JWT_TOKEN = <YOUR TOKEN HERE> <- CHANGE THIS
const TCP_HOST = '127.0.0.1'
const TCP_PORT = 12345

class STALKERConnection {
    constructor() {
        this.client = null;
        this.isConnected = false;
        this.pendingCommands = [];
        
        // Try to connect immediately
        this.connect();
        
        // Reconnect every 5 seconds if disconnected
        setInterval(() => {
            if (!this.isConnected) {
                this.connect();
            }
        }, 5000);
    }

    connect() {
        if (this.client) {
            this.client.destroy();
        }

        this.client = new net.Socket();
        
        this.client.connect(TCP_PORT, TCP_HOST, () => {
            console.log('Connected to STALKER server');
            this.isConnected = true;
            
            // Send any pending commands
            while (this.pendingCommands.length > 0) {
                const command = this.pendingCommands.shift();
                this.sendCommand(command);
            }
        });

        this.client.on('error', (err) => {
            console.error('STALKER connection error:', err.message);
            this.isConnected = false;
        });

        this.client.on('close', () => {
            console.log('STALKER connection closed');
            this.isConnected = false;
        });
    }

    sendCommand(command) {
        if (!this.isConnected) {
            console.log('Connection not established, queuing command:', command);
            this.pendingCommands.push(command);
            return;
        }

        try {
            console.log(`Sending command to STALKER: ${command}`);
            this.client.write(`${command}\n`);
        } catch (err) {
            console.error('Failed to send command:', err.message);
            this.isConnected = false;
            this.pendingCommands.push(command);
        }
    }
}

// Initialize connection
const stalkerConnection = new STALKERConnection();



//! CHANGE THIS TO THE STREAMLABS WEBSOCKET URL IF YOU WANT TO USE THEIR SERVICE, read the documentation here: https://dev.streamlabs.com/docs/socket-api
const ws = new WebSocket(`wss://realtime.streamelements.com/socket.io/?transport=websocket`);

ws.on("open", () => {
    console.log("Connected to StreamElements WebSocket");

    // Authenticate with StreamElements
    ws.send(`42["authenticate", {"method": "jwt", "token": "${JWT_TOKEN}"}]`);

    setInterval(() => { 
        console.log("Sending heartbeat");
        ws.send("42[\"ping\"]");
    }, 30000);
});

let firstMessage = true
let secondMessage = true
ws.on("message", (data) => {
   if(firstMessage){
    firstMessage = false
    const parsed = data.toString();
   console.log("message", parsed, typeof parsed)
   }
   else if (secondMessage){
    secondMessage = false
    const parsed = data.toString();
    console.log("message", parsed, typeof parsed)
   }
   else {

   try { 
    const parsed = JSON.parse(data.toString().substring(2))
    console.log("message", parsed, typeof parsed)

    if(parsed[0] === "event"){ 
        const eventData = parsed[1];

        if(eventData.type === "tip" || eventData.type === "superchat"){
            const amount = eventData.data.amount

            if(amount >= 1 && amount < 5 || amount === 1) {
                stalkerConnection.sendCommand('cheesed_to_meet_you');
            }  else if (amount >= 5 && amount < 10) { 
               stalkerConnection.sendCommand('puppies!');
            }
            else if (amount >= 10 && amount < 15 ) {
                stalkerConnection.sendCommand('bloodsucker_bukkake');
            }
            else if (amount  >= 15) {
                stalkerConnection.sendCommand('spawn_caseoh');
            }
        }
        if(eventData.type === "subscriber"){
            stalkerConnection.sendCommand("you_got_snorked")
        }
    }
    
   } catch (err){ 
    console.error("Error parsing message:", err);
   }

}

})
