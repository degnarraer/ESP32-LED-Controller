export class Model_WifiMode {
    
    constructor(signalName, initialValue, wsManager) {
        this.signalName = signalName;
        this.value = initialValue;
        this.wsManager = wsManager;
        this.wsManager.registerListener(this);
        this.setValue(initialValue, false);
        this.debounceDelay = 1000;
        this.updateWebSocketTimeout = null;
    }

    static values = {
        Station: 'Station',
        AccessPoint: 'AccessPoint',
        Unknown: 'Unknown',
        Count: 'Count'
    };

    cleanup() {
        this.wsManager.unregisterListener(this);
    }

    getSignalName() {
        return this.signalName;
    }

    onMessage(newValue) {
        console.debug(`Message Rx for: "${this.signalName}" with value: "${newValue}"`);
        this.setValue(newValue);
    }

    setValue(newValue, updateWebsocket = true) {
        console.log(`Set Value for Signal: "${this.signalName}" to "${newValue}"`);
        if (Object.values(Model_WifiMode.values).includes(newValue)) {
            this.value = newValue;
            this.updateHTML();
        } else {
            console.error(`"${this.signalName}" Unknown Value: "${newValue}"`);
            throw new Error(`Invalid Value for ${this.signalName}: ${newValue}`);
        }
        if(updateWebsocket){
            this.scheduleWebSocketUpdate();
        }
    }

    scheduleWebSocketUpdate() {
        console.log(`Schedule Update: "${this.signalName}" to "${this.value}"`);
        if (!this.updateWebSocketTimeout) {
            this.sendWebSocketUpdate();
            this.updateWebSocketTimeout = setTimeout(() => {
                this.sendWebSocketUpdate();
                this.updateWebSocketTimeout = null;
            }, this.debounceDelay);
        }
    }

    sendWebSocketUpdate() {
        console.log(`sendWebSocketUpdate: "${this.signalName}" to "${this.value}"`);
        this.wsManager.Send_Signal_Value_To_Web_Socket(this.getSignalName(), this.toString());
    }

    getValue() {
        return this.value;
    }

    toString() {
        switch (this.value) {
            case Model_WifiMode.values.Station:
                return 'Station';
            case Model_WifiMode.values.AccessPoint:
                return 'AccessPoint';
            default:
                return 'Unknown';
        }
    }

    fromString(str) {
        switch (str) {
            case '0':
            case 'Station':
                this.setValue(Model_WifiMode.values.Station);
                break;
            case '1':
            case 'AccessPoint':
                this.setValue(Model_WifiMode.values.AccessPoint);
                break;
            default:
                this.setValue(Model_WifiMode.values.Unknown);
                break;
        }
    }
    
    updateHTML() { 
        console.log(`Updating HTML for Signal: "${this.signalName}"`);
        var elementsWithDataValue = document.querySelectorAll(`[data-Signal="${this.signalName}"]`);
        if (elementsWithDataValue.length === 0) {
            console.error(`"${this.signalName}": No Signals Found!`);
        }
        
        elementsWithDataValue.forEach((element) => {
            if (element.tagName.toLowerCase() === "input" && element.type.toLowerCase() === "radio") {
                // Check if this radio button's value matches the current model value
                element.checked = (element.value === this.value);
                console.log(`"${this.signalName}" Controlled RadioButton "${element.id}" Updated to: "${this.value}"`);
            } else {
                console.error(`"${this.signalName}" Unsupported Element!`);
            }
        });
    }
}