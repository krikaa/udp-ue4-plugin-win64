# UDPCommunication for UE5 #

UDPCommunication is a plugin for UE5 that implements simple UDP communication. This is an upgraded variant of the [lightweight UDPCommunication UE4 plugin by TalTech CIS](https://github.com/is-centre/udp-ue4-plugin-win64).

Like the lightweight version, this upgraded variant can be still used for demanding applications like real-time communication. However, if performance is crucial, it is currently recommended to use the light version, as this version has yet to be extensively tested.

## [Lightweight](https://github.com/is-centre/udp-ue4-plugin-win64) vs Upgraded ##

**Lightweight:**

The lightweight version of the UDPCommunication plugin requires setting up Visual Studio and having some understanding of C++. To change the sent/received packet structure, the source code must be edited and recompiled. To create a sender and receiver, separate actors must be added to the level with the corresponding parent class. 

Advantages:
1. Proven reliability and functionality - used in many demanding projects
2. Lightweight - little code to maintain, easier to prevent bugs and optimize

The lightweight version has been used in Alpha Control Lab (https://a-lab.ee/) and Re:creation VR&AR Lab (https://recreation.ee/) primarily for real-time communication with MATLAB/Simulink software.

Application example: https://recreation.ee/r/about/2018-11-ut/02/

**Upgraded:**

Unlike the lightweight version, the upgraded variant does not typically require the setup of a coding environment (e.g. Visual Studio) and can be used directly in a blueprint project. It features a selection of most primitive data types, arrays and the flexibility to add more complex data types, by converting them to a byte stream or optionally using the older method by manually coding the serialization of said types.

Advantages:
1. Easier to use - packet structure can be changed in the Unreal Editor details panel
2. Faster to use - no need to recompile code, relaunch Unreal or set up coding environments
3. Flexible - added modularity makes it easy to swap, save and use multiple packet structures and senders/receivers

## Installation ##

If your project might benefit more with the lightweight version, refer to the guides in the [repository by TalTech CIS](https://github.com/is-centre/udp-ue4-plugin-win64)
**The following instructions only apply for the upgraded version.**

1. Create a **Plugins** folder in your UE5 project root if it does not already exist. The plugin can be used even in case of purely blueprint-based projects.
2. Copy the UDPCommunication folder from **5.xx** into the new folder where **xx** is your UE5.xx engine version.
3. The plugin is automatically activated and its contents can now be used. Try relaunching Unreal Engine, if UDPCommunication is not appearing in the plugins list. 

## Setting up a receiver/sender instance ##

**First things to consider:**
Both the sender and receiver are **Actor Components**. This means that you can add UDP functionality to the actor of your choice:
* If you want to send and/or receive data that is only associated with one UE actor, it is recommended that you add the components directly to the actor you want to use the communciation with. 
* If you want to send data originating from multiple actors, you can pick one or create a seperate actor that is responsible for the communication, in which you can add the components. You can use the other actor's data for sending/receiving by referencing to them in the blueprint.

1. Create an **Actor** blueprint for the actor responsible for the communication.
2. In the blueprint editor, add the component **UDPSender** and/or **UDPReceiver**, depending on which functionalities do you need.
3. Initialize the sender and/or receiver using the blueprint nodes **Start UDP Sender** and **Start UDP Receiver**. Generally you should connect them to the On Begin Play event. Fill the necessary node fields: input the source/destination IP and port, name the socket. 

## Changing the data structure ##

The plugin allows to create multiple packet structures to send or receive by editing map fields in a special **Data Asset** type called **UDPPacketStructure**.

1. Open the content browser and navigate to **UDPCommunication Content**. Duplicate the data asset named **Template_PacketStructure** to your desired location and rename it to your liking.
2. Customize the packet structure by opening the data asset and changing the settings.
	* Add more data fields by clicking the **+** icon.
	* You can rename the default map field names (Field 1, 2...) to better understand what data the field carries.
	* Specify the data type from the list of option and fill the other details
	* You can also choose to create an array instead by ticking **Is Array** in the options. The created data pin will then only accept arrays and you will have to break them down on your own.
	* Choosing the type **Custom** will use the legacy method of defining a data structure by editing the source code. This requires the setup of a coding environment. See **CustomUDPData.h** for more details. For examples, refer to the [lightweight version](https://github.com/is-centre/udp-ue4-plugin-win64) of the UDPCommunication plugin.
3. Save the changes.

## Sending data ##



## Receiving data ##



## Updating the plugin ##



## Next steps ##

* Instruction videos for the newer version.

## Credits ##

Original code: UE4 user Rama (see https://wiki.unrealengine.com/UDP_Socket_Sender_Receiver_From_One_UE4_Instance_To_Another)

Update for real-time communication: Ralf Anari, TalTech University

Plugin format, packaging, and further updates: Aleksei Tepljakov & Saleh Alsaleh, TalTech University

New features added with the upgraded version: Kristjan Kaal
