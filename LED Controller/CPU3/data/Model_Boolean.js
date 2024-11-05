export class Model_Boolean {
    
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
        0: '0',
        f: '0',
        F: '0',
        false: '0',
        False: '0',
        1: '1',
        t: '1',
        T: '1',
        true: '1',
        True: '1',
        Count: 'Count'
    };

    cleanup() {
        this.wsManager.unregisterListener(this);
        if (this.debounceTimer) {
            clearInterval(this.debounceTimer);
        }
    }

    getListnerName() {return this.signalName;}
    onOpen(){}
    onClose(){}
    onError(){}
    onMessage(newValue) {
        console.debug(`Message Rx for: "${this.signalName}" with value: "${newValue}"`);
        this.setValue(newValue);
    }
    
    setValue(newValue, updateWebsocket = true) {
        console.log(`ESP32 Model: ESP32 Model: Set Value for Signal: "${this.signalName}" to "${newValue}"`);
        if (typeof newValue === 'boolean') {
            newValue = newValue ? Model_Boolean.values.True : Model_Boolean.values.False;
        }
        if (Object.values(Model_Boolean.values).includes(newValue)) {
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
        console.log(`ESP32: ESP32: Schedule Update: "${this.signalName}" to "${this.value}"`);
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
        this.Send_Signal_Value_To_Web_Socket(this.getListnerName(), this.toString());
    }

    Send_Signal_Value_To_Web_Socket(signal, value) {
        if (this.wsManager) {
            if (signal && value) {
                var Root = {};
                Root.SignalValue = {};
                Root.SignalValue.Id = signal.toString();
                Root.SignalValue.Value = value.toString();
                var Message = JSON.stringify(Root);
                console.log('ESP32 Web Socket Tx: \'' + Message + '\'');
                this.wsManager.send(Message);
            } else {
                console.error('Invalid Call to Update_Signal_Value_To_Web_Socket!');
            }
        } else {
            console.error('Null wsManager!');
        }
    }

    getValue() {
        return this.value;
    }

    toString() {
        switch (this.value) {
            case Model_Boolean.values.False:
                return '0';
            case Model_Boolean.values.True:
                return '1';
            default:
                return 'Unknown';
        }
    }

    fromString(str) {
        switch (str) {
            case '0':
            case 'false':
            case 'False':
                this.setValue(Model_Boolean.values.False);
                break;
            case '1':
            case 'true':
            case 'True':
                this.setValue(Model_Boolean.values.True);
                break;
            default:
                this.setValue(Model_Boolean.values.False);
                break;
        }
    }
    
    updateHTML() {
        console.log(`Updating HTML for Signal: "${this.signalName}"`);
        var elementsWithDataValue = document.querySelectorAll(`[data-Signal="${this.signalName}"]`);
        if(elementsWithDataValue.Count === 0){
            console.error(`"${this.signalName}": No Signals Found!`);
        }
        elementsWithDataValue.forEach((element) => {
            if (element.tagName.toLowerCase() === "input" && element.type.toLowerCase() === "checkbox") {
                element.checked = this.value === Model_Boolean.values.True;
                console.log(`"${this.signalName}" Controlled CheckBox "${element.id}" Updated to: "${this.value}"`);
            } else if (element.tagName.toLowerCase() === "button" ) {
                console.log(`"${this.signalName}" Button "${element.id}" Update to HTML not allowed: "${this.value}"`);
            } else {
                console.error(`"${this.signalName}" Unsupported Element!`);
            }
        });
    }
}
