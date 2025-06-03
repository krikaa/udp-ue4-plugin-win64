# Introduction to ***UDPCommunication*** for UE5 #

**UDPCommunication** is an **Unreal Engine 5** plugin that implements simple UDP communication. This is an upgraded variant of the [lightweight UDPCommunication UE4 plugin](https://github.com/is-centre/udp-ue4-plugin-win64) by TalTech Centre of Intelligent Systems.

Like the lightweight version, this upgraded variant can be still used for demanding applications like **real-time communication**. However, if performance is crucial, it is currently recommended to use the light version, as this version has yet to be extensively tested.

## Differences between versions ##

### Lightweight: ###

The [lightweight](https://github.com/is-centre/udp-ue4-plugin-win64) version of the UDPCommunication plugin requires setting up Visual Studio and having some understanding of C++. To change the sent/received packet structure, the source code must be edited and recompiled. To create a sender and receiver, separate actors must be added to the level with the corresponding parent class. 

The lightweight version has been used in [Alpha Control Lab](https://a-lab.ee/) and [Re:creation VR&AR Lab](https://recreation.ee/) primarily for real-time communication with MATLAB/Simulink software.

[Application example](https://recreation.ee/r/about/2018-11-ut/02/) at Re:creation.

<ins>Advantages:</ins>
* Proven reliability and functionality - used in many demanding projects
* Lightweight - little code to maintain, easier to prevent bugs and optimize

### Upgraded: ###

Unlike the lightweight version, the upgraded variant does not typically require the setup of a coding environment (e.g. Visual Studio) and can be used directly in a blueprint project. It features a selection of most primitive data types, arrays and the flexibility to add more complex data types, by converting them to a byte stream or optionally using the older method by manually coding the serialization of said types.

<ins>Advantages:</ins>
* Easier to use - packet structure can be changed in the Unreal Editor details panel
* Faster to use - no need to recompile code, relaunch Unreal or set up coding environments
* Flexible - added modularity makes it easy to swap, save and use multiple packet structures and senders/receivers

# Setting up and using the plugin #

If your project might benefit more with the lightweight version, refer to the guides in the [repository by TalTech CIS](https://github.com/is-centre/udp-ue4-plugin-win64)
> [!IMPORTANT]
> The following instructions only apply for the upgraded version.

## Installation ##

1. Create a ***Plugins*** folder in your UE5 project root if it does not already exist. The plugin can be used even in case of purely blueprint-based projects.
2. Copy the UDPCommunication folder from ***5.xx*** into the new folder where ***xx*** is your ***UE5.xx*** engine version.
3. The plugin is automatically activated and its contents can now be used. Try relaunching Unreal Engine if UDPCommunication is not appearing in the plugins list. 

## Setting up a sender/receiver instance ##

Both the sender and receiver are **Actor Components**. This means that you can add UDP functionality to the actor of your choice.

> [!IMPORTANT]
> * If you want to send and/or receive data that is only associated with one UE actor, it is recommended that you add the components directly to the actor you want to use the communciation with.
> * If you want to send data originating from multiple actors, you can pick one or create a seperate actor that is responsible for the communication, in which you can add the components. You can use the other actor's data for sending/receiving by referencing to them in the blueprint.


1. Create an **Actor** blueprint for the actor responsible for the communication.
2. In the blueprint editor, add the component ***UDPSender*** and/or ***UDPReceiver***, depending on which functionalities do you need.
3. Initialize the sender and/or receiver using the blueprint nodes ***Start UDP Sender*** and ***Start UDP Receiver***. Generally you should connect them to the On Begin Play event. Fill the necessary node fields: input the source/destination IP and port, name the socket. 

## Changing the data structure ##

The plugin allows to create multiple packet structures to send or receive by editing map fields in a special **Data Asset** type named ***UDPPacketStructure***.

1. Open the content browser and navigate to **UDPCommunication Content**. Duplicate the data asset named ***Template_PacketStructure*** to your desired location and rename it to your liking.
2. Customize the packet structure by opening the data asset and changing the settings.
	* You can add more data fields by clicking the **+** icon.
	* You can rename the default map field names (Field 1, 2...) to clarify what data the field carries.
	* Specify the data type from the list of options and fill the other details if necessary. See [Data types](#data-types) for more information.
	* You can also choose to create an array instead by ticking ***Is an Array***. See the section [Arrays](#arrays) for more information.
	* Choosing the type ***Custom*** will use the legacy method of defining a data structure by editing the source code. See the section [Custom](#custom) for more information.
3. Save the changes.

![Screenshot of Unreal Editor depicting the editing of the UDPPacketStructure data asset.](/GitAssets/StructureExample)

## Sending data ##

> [!IMPORTANT]
> To send data, the UDPSender must first be [initialized](#setting-up-a-sender/receiver-instance).

Usually the following nodes should be a part of the ***Event Tick*** flow.

1. Connect the node ***Create UDP Packet***. Select the packet structure you wish to use from the drop-down, this will create the data pins. Connect the data sources to the pins accordingly.
2. Connect the node ***Send UDP Packet***. Connect the previously created packet to the pin ***UDPPacket*** and the initialized instance of ***UDPSender*** to the pin ***Target***.

![Screenshot of an Unreal Engine blueprint using the UDPCommunication plugin to initialize and send data using UDPSender.](/GitAssets/SenderExample)

## Receiving data ##

> [!IMPORTANT]
> To receive data, the UDPReceiver must first be [initialized](#setting-up-a-sender/receiver-instance).

Usually the following nodes should be a part of the ***Event Tick*** flow.

1. Connect the node ***Is New Data Ready?***. Connect the initialized instance of ***UDPReceiver*** to the pin ***Target***.
2. Connect the node ***Get UDP Packet*** to the previous node's ***Ready*** pin. Connect the initialized instance of ***UDPReceiver*** to the pin ***Target***.
3. Connect the node ***Break UDP Packet***. Connect the returned value from the previous node to the pin ***UDPPacket***. Select the packet structure you wish to use from the drop-down, this will create the data pins. Connect the data sources to the pins accordingly.

![Screenshot of an Unreal Engine blueprint using the UDPCommunication plugin to initialize and send data using UDPReceiver.](/GitAssets/ReceiverExample)

# Additional information #

Here are some additional details about the plugin, like how to send/receive certain data types in outer applications correctly or how to update the plugin in case a specific release is not available.

## Data types ##

Sending and receiving most data types is straightforward: pack and unpack them in an outer platform using the correct length. However, knowing the length of some types is not as simple, like strings.

Here is a list of all the usable data types and their lengths:

| Type      | Length 
| --------- | -------
| Boolean   | 8 bits
| Byte      | 8 bits
| Integer   | 32 bits
| Integer64 | 64 bits
| Float     | 32 bits
| Double    | 64 bits
| String    | 32 bit header + 8 bits per character ([See the *Strings* section](#strings))
| Vector    | 192 bits (3 x 64-bit double)
| Rotator   | 96 bits (3 x 32-bit float)
| Transform | 480 bits (3 x 64-bit double, 3 x 32-bit float, 3 x 64-bit double)
| Custom    | [See the *Custom* section](#custom)

### Strings ###

Since strings are with a dynamic size, making it difficult to use them in a packet with a fixed size, the way of sending them has been standardized. 

When selecting the string type in the UDPPacketStructure data asset, you must also enter the **max string length**. The **max string length** determines how long the string can be, but it can still be shorter than the defined length. If the string is shorter, padding (in the form of 0's) will be added to keep the whole structure aligned.

> [!IMPORTANT]
> For faster processing, the actual field length of the string is **rounded up to the nearest 4th byte**, but only if there are any other data fields after the string. Make sure to take this into account when processing the string in an outer platform!<br>
> Example: max string length = 5 -> field length  = 8

To further help sending/receiving data outside UE, strings have a **32-bit integer header** before the string data. This header contains the real length (not the max length!) of the string. Use this to pack and unpack the string correctly, avoiding empty characters.

Both the real string length and max string length are equal to **one 8-byte character**. Meaning the length 3 is equal to 24 bytes.

> [!IMPORTANT]
> The strings are encoded in UTF-8 for optimal data usage. This however means that some unstandard characters (like with umlauts) may take more than 8 bytes. Make sure to check the length of the characters if you are not using the default Latin alphabet and increase the max string length if necessary!<br>
> Example: ä takes double the size - 16 bytes

### Arrays ###

To send massive amounts of data with the same type, it may be more convenient to send the data as an array to prevent the blueprint from becoming cluttered. By selecting ***Is an Array*** in a packet structure field, the created data pin on nodes will only accept arrays. You will have to make the arrays or break them down on your own by using standard pre-existing nodes.

Selecting *Is an Array* only changes the way how data is handled in the blueprints. In the end the data is still packed the same way as it would be packed when creating multiple fields with the same type, with nothing indicating that it was an array in the first place. 

The field length will be equal to the **element count x length of the data type**.

> [!TIP]
> When receiving multiple fields with the same type from the outside, you can select *Is an Array* with the correct length and the fields will be put together for easier handling. 

### Custom ###

Selecting the type ***Custom*** will use the legacy method of defining a data structure by editing the source code. 

> [!IMPORTANT]
> This requires the setup of a coding environment.

In ***CustomUDPData.h*** are two sections: one for declaring the variables in a structure, the second for serializing the declared variables. To change the types, you must edit both.

To use the coded structure, you must add the *Custom* type as a field to the *UDPPacketStructure* data asset. This will create a pin to the nodes *Create UDP Packet* and *Break UDP Packet* which accepts a connection to the struct *CustomStructure*. To make or break this struct, use the nodes *Make Custom Structure* and *Break Custom Structure* similarly to the other nodes. This opens up the opportunity to use complex data types by implementing your own serialization methods.

 For more details and examples, refer to the [lightweight version](https://github.com/is-centre/udp-ue4-plugin-win64) of the UDPCommunication plugin, as the *Custom* type is identical to the older solution of declaring a packet structure.

> [!TIP]
>  If you want to use data types that are unsupported by the plugin, but do not want to use the *Custom* type, you may also try braking the unsupported type down to a byte array using pre-existing UE nodes and then adding a byte array to the packet structure to send the data.

## Updating the plugin ##

If there is no release of UDPCommunication on your preferred version, you can try using a simple plugin updater tool: ***UpdatePlugin.bat***. It automatically calls the Unreal Automation Tool CLI commands and creates a configuration file named ***UpdaterConfig.ini*** to simplifying updating. It can be used for any plugin, not only UDPCommunication. 

> [!IMPORTANT]
> To use the updater, you need to set up Visual Studio and install the UE version you want to update to, as Unreal Automation Tool uses Microsoft's build tools to update rebuild the plugin binaries.

[[Download the updater tool](https://github.com/krikaa/udp-ue5-plugin-win64/tree/master-UE5.5/UpdatePlugin.bat)]

# Development #

If you encounter any bugs or have suggestions for additional functionality, it would be greatly appreciated if you contribute by leaving them in the repository's [Issues](https://github.com/krikaa/udp-ue5-plugin-win64/issues).

## Future improvements ##

* Add instruction videos for the newer version.
* Add real-life examples of the upgraded plugin.

## Credits ##

* Original code: UE4 user Rama (see https://wiki.unrealengine.com/UDP_Socket_Sender_Receiver_From_One_UE4_Instance_To_Another)
* Update for real-time communication: Ralf Anari, TalTech University
* Plugin format, packaging, and further updates: Aleksei Tepljakov & Saleh Alsaleh, TalTech University
* New features added with the upgraded version: Kristjan Kaal, TalTech University
