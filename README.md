# Hardware to Software Integration Framework (HSIF)

The contents of this readme can be viewed in proper format at this [link](https://github.com/Itreau96/cse593-hsif/blob/master/README.md).

HSIF is a light-weight C/C++ framework for integrating hardware-in-the-loop testing and computing platforms. Robotics and autonomous vehicles are just a few examples of business domains that greatly benefit (and necessitate) realistic testing environments and platforms. However, most platforms that support this form of testing are rigid and singular in purpose. Aside from the safety critical applications mentioned, HSIF can lend itself to a variety of different software domains (entertainment, video games, virtual reality, networking, etc.). With the IOT domain growing at an exponential pace, custom hardware/software integration platforms are becoming more and more of a necessity. At its core, HSIF values speed, modularity, efficiency, and extensibility.

## Prerequisites

Repository library dependencies are listed below:
* hsif
    * Built using CMake in Visual Studio 2019 (v16.7.1). Ensure CMake and C++ support is installed with your version of c++. Only supported on Microsoft Windows at present.
* rpi
    * Rpi HSIF endpoints: Both endpoint examples require Python 3+ to run. rpi_endpt_emu.py can be run in a Raspberry Pi QEMU instance or any machine with Python interpreter installed. The rpi_endpt_hw.py file must be run on a physical Raspberry Pi device.
    * QEMU: The folder contains two example scripts for setting up a TAP network bridge in Linux. The example in this project was run using QEMU in an Ubuntu Linux VirtualBox instance. See installation instructions below for setting up this environment.
* unity
    * The scripts contained in this folder were tested using Unity v2018.3. The demonstration scene leveraging these scripts can be run using this version of Unity.

## Installation

### hsif

1. Navigate to the hsif folder in the current repository.
2. Open the hsif folder in Visual Studio 2019. **(NOTE: users may need to modify CMakeSettings.json to match their build environment)**
3. If you wish to change the default configuration of the HSIF server, you can modify main.cpp within the src folder (default: localhost, port 8888)
4. Build by selecting Build->Build All in the Visual Studio toolbar.
5. You can now execute the .exe generated within the "out" build folder or execute from Visual Studio (Debug->Start Debugging for example)

### unity

1. Download the Unity installer from [here](https://store.unity.com/?_ga=2.188073128.1846184033.1607300555-438931787.1606711502#plans-individual). When selecting a Unity version from the installer, select v2018.3.
3. Open unity and create a new, empty, 3D project.
4. Once the editor has opened, you can import the Unity package from Assets->Import Package->Custom Package...
5. Navigate to the unity folder and select the hsifDemo.unitypackage file.
6. Import all resources from package into current scene.
7. You can modify the demonstration by selecting the "Pane" object from the project hierarchy and changing the HSIF Display script attributes.

### rpi

**Emulation:**

**NOTE: Emulation setup is fairly complex and may not work if your machine is not powerful enough to support it. The "Running Examples" section demonstrates how the rpi endpoint can be used outside of emulation if simple testing desired.**
1. The first step is to install a Linux development environment. In our example we are using the most recent version of Ubuntu. The Ubuntu image can be installed from [here](https://ubuntu.com/download/desktop).
2. We are using VirtualBox to boot our Ubuntu image. You can install the latest VirtualBox installation from [here](https://www.virtualbox.org/).
3. Open VirtualBox and create a new Ubuntu virtual machine using the image downloaded. Please consult the internet for this process as it may have changed at time of reading. ([these](https://brb.nci.nih.gov/seqtools/installUbuntu.html) instructions were used at time of writing). Select a RAM and disk size that is reasonable considering your machine's resources.
4. Before starting the VirtualBox instance, ensure network bridging has been setup correctly. Within the vm settings, ensure the Network's settings tab is using a Bridged Adapter.
5. Open the Ubuntu virtual machine and install QEMU. Instructions for doing so can be found [here](https://azeria-labs.com/emulate-raspberry-pi-with-qemu/) (stop after Troubleshooting section). HSIF was tested using the kernel-qemu-4.4.34-jessie image.
6. Copy the setup_tap.sh, start-qemu.sh, and rpi_endpt_emu.py scripts from the rpi folder to your Ubuntu Linux filesystem. This can be accomplished using an scp command to transfer files into the vm while it is powered on. You will need to know the ip address of the Ubuntu instance before calling scp.
7. You will setup the network bridge between QEMU and the Windows machine using an additional network bridge. Start by identifying the name of the Ubuntu network connection. This can be found using an ifconfig call (sometimes it is eth1, in our case it is enp0s3). Replace all instances of enp0s3 in the setup_tap.sh script. Run setup_tap.sh to create the network bridge **(NOTE: this will need to be done everytime the vm is rebooted)**.
8. You can now run the QEMU instance by calling start-qemu.sh (ensure qemu-rpi-kernel-master is in the same directory as start-qemu.sh).
9. You can now run and interact with the raspberry pi QEMU instance. 
10. Once you determine the Ip of your QEMU emulation on the network, you can scp the rpi_endpt_qemu.py script to the QEMU filesystem from Ubuntu or Windows host. 

**Hardware:**

1. Ensure your Raspberry Bi 3 Model B is connected to the same network as the Windows machine that will run the HSIF server and Unity. 
2. Determine the Ip of your Raspberry Pi on the network using an ifconfig command.
3. From Windows, you can scp the rpi_endpt_hw.py file to the Rpi filesystem.
4. The Rpi is now ready to send and receive messages using the endpoint file. 

## Running Examples

### Quickstart

The quickstart example is ideal for demonstrating HSIF server functionality on Windows alone. The emulation configuration and setup can be difficult and resource intensive. This example provides a simple demonstration that can be executed using Windows alone. 

**Windows Only Quickstart**

1. Open the HSIF server project in Visual Studio (setup described in Installation section).
2. Run in Visual Studio by selecting Debug->Start Debugging or by running the generated .exe directly.
3. Open a command line window with Python3 in its path. Navigate to the rpi repository directory.
4. Run the endpoint using the following command: ```python .\rpi_endpt_emu.py localhost 8888 rpi sim 2```
5. Open a second terminal window with Python3 in its path. Navigate to the rpi repository directory.
6. Run the endpoint using the following command: ```python .\rpi_endpt_emu.py localhost 8888 sim rpi 2```
5. You should see output within all three software consoles demonstrating simulated temperature data being sent between both python endpoints via HSIF server.

**Windows + Unity Quickstart**

1. Open the HSIF server project in Visual Studio (setup described in Installation section).
2. Run in Visual Studio by selecting Debug->Start Debugging or by running the generated .exe directly.
3. Open the Unity project (setup described in Installation section).
4. Run the project. You should see the connection appear in the HSIF console Window.
5. Open a command line window with Python3 in its path. Navigate to the rpi repository directory.
6. Run the endpoint using the following command: ```python .\rpi_endpt_emu.py localhost 8888 rpi sim 2```
7. You should see output within all three software consoles demonstrating Unity receiving simulated temperature data from the python endpoint via the HSIF server.

### Emulation

1. Open the HSIF server project in Visual Studio (setup described in Installation section).
2. Run in Visual Studio by selecting Debug->Start Debugging or by running the generated .exe directly.
3. Open the Unity project (setup described in Installation section).
4. Run the project. You should see the connection appear in the HSIF console Window.
5. Open the QEMU instance (setup described in Installation section).
6. Open a terminal window in QEMU and navigate to the rpi_endpt_emu.py file.
7. Determine the ip address of the Windows host using an ipconfig call on the machine. 
6. Run the endpoint using the following command: ```python .\rpi_endpt_emu.py (windows-ip) 8888 rpi sim 2```
7. You should see output within all three software consoles demonstrating Unity receiving simulated temperature data from the QEMU endpoint via the HSIF server.

### Hardware

1. Open the HSIF server project in Visual Studio (setup described in Installation section).
2. Run in Visual Studio by selecting Debug->Start Debugging or by running the generated .exe directly.
3. Open the Unity project (setup described in Installation section).
4. Run the project. You should see the connection appear in the HSIF console Window.
5. Power on the Rpi board (setup described in Installation section).
6. Open a terminal window on the Rpi OS and navigate to the rpi_endpt_emu.py file.
7. Determine the ip address of the Windows machine using an ipconfig call on the machine. 
6. Run the endpoint using the following command: ```python .\rpi_endpt_emu.py (windows-ip) 8888 rpi sim 2```
7. You should see output within all three software consoles demonstrating Unity receiving temperature data from the Rpi endpoint via the HSIF server.