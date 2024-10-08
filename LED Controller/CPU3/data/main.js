import { WebSocketManager } from './WebSocket.js';
import { Model_InputSource } from './Model_InputSource.js';
import { Model_OutputSource } from './Model_OutputSource.js';
import { Model_ConnectionStatus } from './Model_ConnectionStatus.js'
import { Model_Boolean } from './Model_Boolean.js';
import { Model_Numeric } from './Model_Numeric.js';
import { Model_Text } from './Model_Text.js';
import { Model_WifiMode } from './Model_WifiMode.js';
import { Model_BtDeviceSelector } from './Model_BtDeviceSelector.js'

const wsManager = new WebSocketManager();

//var loadingAnimation;


export const SinkName = new Model_Text('Sink_Name', 'LED Tower of Power', wsManager);
export const CurrentSoundInputSource = new Model_InputSource('Input_Source', Model_InputSource.values.OFF, wsManager);
export const BT_SinkEnable = new Model_Boolean('BT_Sink_En', Model_Boolean.values.False, wsManager);
export const Sink_ConnectionStatus = new Model_ConnectionStatus('Sink_Conn_State', 'Disconnected', wsManager);
export const Sink_Connect = new Model_Boolean('Sink_Connect', Model_Boolean.values.False, wsManager);
export const Sink_Disconnect = new Model_Boolean('Sink_Disconnect', Model_Boolean.values.False, wsManager);
export const Sink_Auto_Reconnect = new Model_Boolean('BT_Sink_AR', Model_Boolean.values.False, wsManager);

export const SourceName = new Model_Text('Source_Name', '', wsManager);
export const CurrentSoundOutputSource = new Model_OutputSource('Output_Source', Model_OutputSource.values.OFF, wsManager);
export const BluetoothDeviceSelector = new Model_BtDeviceSelector('Scan_BT_Devices', wsManager);
export const BT_SourceEnable = new Model_Boolean('BT_Source_En', Model_Boolean.values.False, wsManager);
export const Source_ConnectionStatus = new Model_ConnectionStatus('Src_Conn_State', 'Disconnected', wsManager);
export const Source_Connect = new Model_Boolean('Src_Connect', Model_Boolean.values.False, wsManager);
export const Source_Disconnect = new Model_Boolean('Src_Disconnect', Model_Boolean.values.False, wsManager);
export const Source_Reset = new Model_Boolean('BT_Src_Reset', Model_Boolean.values.False, wsManager);
export const Source_Auto_Reconnect = new Model_Boolean('BT_Source_AR', Model_Boolean.values.False, wsManager);


export const WIFI_Mode = new Model_WifiMode('WIFI_Mode', Model_WifiMode.values.Unknown, wsManager);
export const Host_Name = new Model_Text('Host_Name', 'ENTER VALUE', wsManager);
export const STA_SSID = new Model_Text('STA_SSID', 'ENTER VALUE', wsManager);
export const STA_Password = new Model_Text('STA_Password', 'ENTER VALUE', wsManager);
export const AP_SSID = new Model_Text('AP_SSID', 'ENTER VALUE', wsManager);
export const AP_Password = new Model_Text('AP_Password', 'ENTER VALUE', wsManager);
export const Wifi_Restart = new Model_Boolean('Wifi_Restart', Model_Boolean.values.False, wsManager);

export const Amplitude_Gain = new Model_Numeric('Amp_Gain', 2.0, wsManager);
export const FFT_Gain = new Model_Numeric('FFT_Gain', 2.0, wsManager);


//Compatible Devices
var compatibleDevices = [
	{ name: "Device 1", address: "00:11:22:33:44:55", rssi: -50 },
	{ name: "Device 2", address: "AA:BB:CC:DD:EE:FF", rssi: -60 },
];

let selectedDeviceIndex = -1;

const ConnectionState = 
{
	Disconnected: 0,
	Connecting: 1,
	Connected: 2,
	Disconnecting: 3
}

const ConnectionStateString = 
{
	0: 'Disconnected',
	1: 'Connecting',
	2: 'Connected',
	3: 'Disconnecting'
}

export function showContent(classId, contentId) {
    // Select all elements with the given classId
    var tabContents = document.querySelectorAll('.' + classId);
    
    // Hide all tab contents
    tabContents.forEach(function (tabContent) {
        tabContent.classList.remove('active');
    });
    
    // Update the main menu heading
    var heading = document.getElementById("mainMenu_Heading");
    if (heading) {
        heading.innerText = contentId;
    } else {
        console.warn("Element with ID 'mainMenu_Heading' not found.");
    }
    
    // Show the selected tab content
    var selectedContent = document.getElementById(contentId);
    if (selectedContent) {
        selectedContent.classList.add('active');
    } else {
        console.warn("Element with ID '" + contentId + "' not found.");
    }
}

export function openNav() 
{
  document.getElementById('leftSideNavigationMenu').style.width = '200px';
}

export function closeNav()
{
  document.getElementById('leftSideNavigationMenu').style.width = '0';
}

export function GetElementValue(element) {
	return element.value;
}

export function GetElementDataSignal(element) {
	return element.data-Signal;
}

export function SendValueToWebSocket(signalName, value){
	wsManager.Send_Signal_Value_To_Web_Socket(signalName, value);
}

window.addEventListener('load', onload);
function onload(event)
{
	wsManager.initWebSocket();

	var sink_BT_Auto_ReConnect_Toggle_Button;
	sink_BT_Auto_ReConnect_Toggle_Button = document.getElementById('Sink_BT_Auto_ReConnect_Toggle_Button');
	sink_BT_Auto_ReConnect_Toggle_Button.addEventListener('change', function()
	{
		Sink_Auto_Reconnect.setValue(sink_BT_Auto_ReConnect_Toggle_Button.checked? "1" : "0");
	});

	var source_BT_Reset_Toggle_Button;
	source_BT_Reset_Toggle_Button = document.getElementById('Source_BT_Reset_Toggle_Button');
	source_BT_Reset_Toggle_Button.addEventListener('change', function()
	{
		Source_Reset.setValue(source_BT_Reset_Toggle_Button.checked? "1" : "0");
	});

	var source_BT_Auto_ReConnect_Toggle_Button;
	source_BT_Auto_ReConnect_Toggle_Button = document.getElementById('Source_BT_Auto_ReConnect_Toggle_Button');
	Source_BT_Auto_ReConnect_Toggle_Button.addEventListener('change', function()
	{
		Source_Auto_Reconnect.setValue(source_BT_Auto_ReConnect_Toggle_Button.checked? "1" : "0");
	});
}

function show_Connecting_Modal() {
	const loadingTextElement = document.querySelector('#loadingModal .modal-content h2');
	let dotCount = 0;
	loadingTextElement.textContent = 'Connecting';
	document.getElementById('loadingPage').style.display = 'flex';
	loadingAnimation = setInterval(() => {
		dotCount = (dotCount + 1) % 4;
		loadingTextElement.textContent = 'Connecting' + '.'.repeat(dotCount);
	}, 1000);
}

function setSpeakerImage(value)
{   
	var Image1Source;
	var Image2Source;
	var state = parseInt(value);
	const imageOneElement = document.getElementById('L Speaker Image');
	const imageTwoElement = document.getElementById('R Speaker Image');
	var imageOne = new Image;
	var imageTwo = new Image;
	switch(state)
	{
	  case 0:
		Image1Source = 'Images/L-Speaker-Off.svg';
		Image2Source = 'Images/R-Speaker-Off.svg';
	  break;
	  case 1:
		Image1Source = 'Images/L-Speaker-0.svg';
		Image2Source = 'Images/R-Speaker-0.svg';
	  break;
	  case 2:
		Image1Source = 'Images/L-Speaker-1.svg';
		Image2Source = 'Images/R-Speaker-1.svg';
	  break;
	  case 3:
		Image1Source = 'Images/L-Speaker-2.svg';
		Image2Source = 'Images/R-Speaker-2.svg';
	  break;
	  case 4:
		Image1Source = 'Images/L-Speaker-3.svg';
		Image2Source = 'Images/R-Speaker-3.svg';
	  break;
	  case 5:
		Image1Source = 'Images/L-Speaker-4.svg';
		Image2Source = 'Images/R-Speaker-4.svg';
	  break;
	  case 6:
		Image1Source = 'Images/L-Speaker-5.svg';
		Image2Source = 'Images/R-Speaker-5.svg';
	  break;
	  case 7:
		Image1Source = 'Images/L-Speaker-6.svg';
		Image2Source = 'Images/R-Speaker-6.svg';
	  break;
	  case 8:
		Image1Source = 'Images/L-Speaker-7.svg';
		Image2Source = 'Images/R-Speaker-7.svg';
	  break;
	  case 9:
		Image1Source = 'Images/L-Speaker-8.svg';
		Image2Source = 'Images/R-Speaker-8.svg';
	  break;
	  case 10:
		Image1Source = 'Images/L-Speaker-9.svg';
		Image2Source = 'Images/R-Speaker-9.svg';
	  break;
	  case 11:
		Image1Source = 'Images/L-Speaker-10.svg';
		Image2Source = 'Images/R-Speaker-10.svg';
	  break;
	  case 12:
		Image1Source = 'Images/L-Speaker-11.svg';
		Image2Source = 'Images/R-Speaker-11.svg';
	  break;
	  default:
		Image1Source = 'Images/L-Speaker-0.svg';
		Image2Source = 'Images/R-Speaker-0.svg';
	  break;
	}
	imageTwo.src = Image2Source;
	const imageOnePromise = new Promise((resolve, reject) => {
	  imageOne.src = Image1Source;
	  imageOne.onload = () => {
		resolve();
	  };
	  imageOne.onerror = () => {
		reject('Error loading image one');
	  };
});

const imageTwoPromise = new Promise((resolve, reject) => {
		imageTwo.src = Image2Source;
		imageTwo.onload = () => {
			resolve();
		};
		imageTwo.onerror = () => {
			reject('Error loading image two');
		};
	});  

	Promise.all([imageOnePromise, imageTwoPromise]).then(() => {
		// both images have finished loading
		imageOneElement.src = imageOne.src;
		imageTwoElement.src = imageTwo.src;
	});
}

function handleSpeakerImage(id, value) {
	if(id && value)
	{
		console.log('Received Speaker Image!');
		setSpeakerImage(value);
	}
}

