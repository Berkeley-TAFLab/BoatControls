import QtQuick  
import QtQuick.Window


Window{
	width: 1280 
	height: 720 
	visible: true 

	title: qsTr("Boat Control UI") //Controls the title at the top of the window
	
	
	TAF_Map_View{
		id: mapview
	}

	TAF_Controlbar{
		id:controlBar	
	}



}

