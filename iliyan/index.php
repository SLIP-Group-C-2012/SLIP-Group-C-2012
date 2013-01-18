

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Group C</title>
<style type="text/css">
<!--
-->
</style>
<link href="../css/main.css" rel="stylesheet" type="text/css" />
<!--[if lte IE 7]>
<style>
.content { margin-right: -1px; } /* this 1px negative margin can be placed on any of the columns in this layout with the same corrective effect. */
ul.nav a { zoom: 1; }  /* the zoom property gives IE the hasLayout trigger it needs to correct extra whiltespace between the links */
</style>
<![endif]--></head>
<body>

<div class="container">

    <div class="header"><a href="#"><img src="/teaching/slipc12-13/images/small/DPP_0003.jpg" alt="Insert Logo Here" name="Insert_logo" width="20%" height="90" id="Insert_logo" style="background-color: #8090AB; display:block;" /></a> 
    <!-- end .header --></div>    <div class="sidebar1">
    <ul class="nav">
      <li><a href="/teaching/slipc12-13/index.php">Home</a></li>
      <li><a href="/teaching/slipc12-13/imagegallery.php">Images</a></li>
      <li><a href="/teaching/slipc12-13/altynbek/index.php">Altynbek</a></li>
      <li><a href="/teaching/slipc12-13/iliyan/index.php">Iliyan</a></li>
      <li><a href="/teaching/slipc12-13/lavanya/index.php">Lavanya</a></li>
      <li><a href="/teaching/slipc12-13/martin/index.php">Martin</a></li>
      <li><a href="/teaching/slipc12-13/mike/index.php">Mike</a></li>
      <li><a href="/teaching/slipc12-13/henrik/index.php">Henrik</a>  <!-- end .sidebar1 --></li>
      <li><a href="/teaching/slipc12-13/richard/index.php">Richard</a></li>
    </ul>
</div>

  
  
  <div class="content">
<h1>Iliyan Ivanov (s0952315)</h1>
<h3 class="c0">
  <strong><span><a name="overview" id="overview"></a>An Overview</strong>
</h3>
<p class="c1 c2 c6"><span></span>As a part of Group C in SLIP, I was working on the &quot;Real-time audio and text transmission using a mesh network&quot;. Our project's main idea was to transmit voice through a mesh network using the built-in radio chip. As an addition to this, we have also created a text mode in which users can send text messages through the mesh. Also by using computers connected to the end devices, we can transform these texts to speech and hear the messages on the computer. </p>
<p class="c1 c2 c6">An example of a real-life application of our project is using the mesh to connect to others when other forms of communications fail. For example in a disaster or any other extreme environment.</p>
<p class="c1 c2 c6">My contribution to the project consisted on mainly working on the radio, establishing communication first between two and then more devices by using retransmissions when necessary. I have also worked on the integration of the whole project, testing and obtaining quantitative data, characterizing the system as a whole and general decision making as part of the team.</p>
<p class="c1 c2 c6">I will not be talking about the hardware configuration of the system, as the people reading this document are most probably familiar with it. In the following section I discuss some of the challenges we faced during the scope of the project as well as my contributions to the project.</p>
<h3 class="c0"><a name="firstComm" id="firstComm"></a>First form of communication<span></span></h3>
<p class="c0">At first we were given the ProSpeckz boards (referred to as &quot;devices&quot;) and some example code written by our mentor Janek Mann. As this was all new to me and I didn't have much experience in such low level programming and electronics I found it quite challenging to get a grip on things. However exploring the boards and the application notes was quite interesting.</p>
<p class="c0">After a couple of weeks of reading and exploring Mr. Mann's demo code I was able to modify it in order to implement a very simple form of a communication protocol. Using the demo code and writing a simple sender and receiver functions two devices were able to communicate between themselves. This was quite challenging because after sending a packet a device could not go directly into receiver mode (RX), but first had to go through other states. This is true for switching between transmit mode (TX) to receive mode as well. Figuring out those steps was a bit of a challenge, but I managed to get it working. I should say that I am quite sure that I've added some steps which are probably redundant but I couldn't get it to work otherwise. So the product of my work at this stage was a simple protocol (<a href="send_receive.c"><font color="#FF0000">send_receive.c</font></a>) which implemented the following: </p>
<p class="c0"><em>Device A sending a number to device B (we start from 1). Device B acknowledging the number and sending it back to device A. Then if the first device recognizes that it has sent this number it increases it by 1 and sends it and so on. If not, it repeats the last number.</em></p>
<p class="c0">When the devices print to the computer's screen over the serial we can see that they are actually counting. The output of the communication between the two devices looks like this:</p>
<table width="70%" border="5">
  <tr>
    <td>Send seq: 1<br />     &gt; Received : 1<br />     &gt; Send seq: 1<br />
      Received : 1<br />
      (+1)<br />
      Send seq: 2<br />     &gt; Received : 2<br />     &gt; Send seq: 2<br />
      Received : 2<br />
      (+1)<br />
      Send seq: 3<br />     &gt; Received : 3<br />     &gt; Send seq: 3<br />
      Received : 3<br />
      (+1)<br />
      Send seq: 4<br />
      ...<br />    </td>
  </tr>
</table>
<p class="c0">As we were using the built-in ack-ing system, there were no lost packets. The fact that there were no missing number in the output proved that theory. Also the devices needed to be placed very close to each other because we did not have antennas at that time.</p>
<p class="c0">I should say that I wasn't working alone on figuring out how to use the radio. Martin was also reading the manuals and experimenting with it.</p>
<p class="c0">At that point it was only natural to try and see if I could establish a similar kind of communication between more than two devices.</p>
<h3 class="c0"><a name="secondComm" id="secondComm"></a> Communication between more devices</h3>
<p class="c0 c1"><span></span>When I started enhancing the communication I saw that devices and packets needed to be identifiable is some way. Otherwise the medium would be flooded with packets and the devices would not be able to handle them efficiently. This is why I developed a some sort of packet headers. The size of the packets was a big as the hardware allowed us, which was 32 bytes. The first version of the packets looked like this:</p>
<table width="70%" border="5">
  <tr>
    <td><pre>
  #define MY_ADDR (2)

	typedef struct {
  		uint8_t src;
  		uint8_t dest;
  		uint8_t packetID;
  		uint8_t data[29];
	} Packet_Type __attribute__ ((packed));

	Packet_Type packet;
    </pre></td>
  </tr>
</table>
<p class="c0 c1">First of all I used numbers to identify the devices. Each device had its ADDRESS hardcoded. In the example above we can see that this devices address is set to 2.</p>
<p class="c0 c1">Each packet contained the address of the source device and the destination devices as an unsigned integer. Each of those was a byte long. In this way a device would know who the packet is destined for and therefore make an informed decision whether to accept or retransmit the packet. Another byte was needed for the sequence number of the packet - packetID. This helps to prevent  unnecessary retransmissions of the same packet and flooding the channel. The rest 29 bytes were used to carry the data.</p>
<p class="c0 c1">The logic of this new communication protocol was really straightforward. When a packet is received a device first checks the packet's destination address. If the packet is destined for the current device, it then checks the ID of the packet. If it's a fresh one then it accepts it and deals with the data and if not it just discards it. The way for checking for the packet's freshness is by comparing the ID of the packet with the last received IDs. If they duplicate then the packet is discarded. However if the packet is not for &quot;us&quot; then we retransmit the packet, hoping that through us it can get to its destination. When retransmitting we should first check for the freshness of the packet again, because otherwise the same packet will be passed around by the devices within range forever. </p>
<p class="c0 c1">For testing purposes I have created a simple command for sending a message from one device to another specifying the address. The command was typed on a computer, connected to a device via serial port. It consisted of a simple text message and a destination address. The device sending the packet would fill the remaining (sender's address and ID) automatically. In this way I was able to send packets to the desired devices within range. As we were given antennas the ranges were much larger and therefore I experimented with that and obtained the following results:</p>
<p class="c0 c1"><em>The difference between a network of two devices away from each other and a network of three devices, one of which acting as a relay:</em></p>
<p class="c0 c1"><img src="images/scheme01.png" width="900" height="455" /></p>
<p class="c0">As we can see on the left the two devices are about 20m. away from each other and there is a wall obstructing the communication between them. In this scenario about 60% of the sent packets were not received. However by adding a third device somewhere in the middle improves the link quality greatly. What the test consisted of was sending packets from device2 to device1 and checking the number of successfully received packets compared to the number of sent packets. In the second scenario the Relay device would recognize that the packet is not destined for it and resend it again. Measurements showed that the lost packets rate dropped to 10%. </p>
<p class="c0">Another test I've conducted measured the throughput. According to the manual and the application nodes we should have been able to achieve 256 kBps, but my tests showed only 42 kBps. This is not very surprising as we had the hardware ack-ing system on, the channel was probably not absolutely clear and there are the distance and the wall which obstruct the communication.</p>
<h3 class="c0"><a name="protocol" id="protocol"></a> The Protocol for text and audio</h3>
<p><span class="c0 c1">The job of the protocol was assembling packets and handling the sending and the reception of such. It had to be fast, reliable in order not to drop many packets. The basic protocol would keep the structure of the packets, keep track of their IDs and would have a send and receive functions for communication.</span></p>
<p class="c0 c1"><span></span>After some great work on the DAC, ADC, Microphone and Speakers by the members of our group we were able to transmit audio from one device to another. As these were the first trials for transmitting audio we did not use addressing on the devices. At that point we just had one device transmitting the packets and the other buffering them and playing them. However, while testing we noticed that there was a noticeable delay between transmitting and playing so we spent quite some time investigating that.</p>
<p class="c0 c1">As I was in a way familiar with the radio I wrote another protocol (<a href="proto.c"><font color="#FF0000">proto.c</font></a>) which was a simplified version of the protocol that my team had before. The new protocol one send and one receive function. The send function would take some input data, assemble a packet and transmit it. The receive function would determine whether the packet is for us and handle it appropriately. However the old one had two memcpy() operations and mine had only one. It turned out that this shortened the delays when we tested again. The new measured delay when transmitting audio was 1.5secs, which is still noticeable, but acceptable. The explanation for this is that the additional memory copying of 30 bytes at every packet received was slowing the playback, because memcpy() is a comparatively expensive and slow operation. The rest of the protocol was quite straightforward like before, with the exception of the addressing. We decided we did not need addressing and we would keep track of the packets using the IDs.</p>
<p class="c0 c1">We've also noticed that if we use retransmissions the delay between transmitting and playing back grows very rapidly, so retransmitting audio wasn't an option anymore. This is when we decided to add text messaging. An absolutely reliable service that works on as many devices as necessary. Therefore I modified the protocol by adding a type field to the packet header. The type specifies whether the data in the packet is audio(0) or text(1) so that devices can make informed decisions on how to interpret the data. As well as that I noticed that the IDs get mixed between audio and text, so I implemented separate IDs for each of the two types of data (idT, idA). This is how the packet header in the final version looked like:</p>
<table width="70%" border="5">
  <tr>
    <td><pre>
	typedef struct
	{
		uint8_t packetID;
		uint8_t type;
		uint8_t data[30];
	} Packet_Type __attribute__ ((packed));

	Packet_Type packet;

	uint8_t idA = 1;
	uint8_t idT = 1;
    </pre></td>
  </tr>
</table>
<p class="c0 c1">For testing and debugging we used Cutecom for most of the time. The devices had the feature of printing messages over the serial port and Cutecom printed these on the screen. This is what the Cutecom output looked like when we were printing the IDs of the received audio packets: <a href="cutecom2.log">cutecom.log</a>. IDs range from 0 to 255 as we were using unsigned integers for them. When following the numbers we can easily see that some are missing. This shows that not all packets are received, but since we are sending audio some packet loss is tolerable.</p>
<p class="c0 c1">I would like to state that I am not the only one that has worked on the protocol. After I had written the basic code needed for handling the packets the other team members added a bugfix to the protocol and improved the overall operation of the mesh.</p>
<p class="c0 c1"><span class="c0"><a name="leds" id="protocol2"></a><strong> Other contributions</strong></span></p>
<p class="c0 c1">In order to be able to test the transmission of packets and audio we needed at least two devices, each connected to a computer via serial cable. I found this quite uncomfortable at some times as we only had a limited number of cables and each had one computer. This is why I spend almost a day on trying to create some kind of a signaling system using the three LEDs on the board. However I wasn't successful as I could only get two of the LEDs to work and two wasn't enough for what I had in mind. </p>
<p class="c0 c1">Another thing I worked on in the last hours before our presentation was the demonstration of the text-to-speech part of the GUI. It was already done but as we decided to demonstrate it on the final day, we found that there were some problems with the libraries and we couldn't get it to work. Luckily I was able to run the GUI and the TTS feature in time for the presentation, so we could show the audience how the computer reads the text sent through the mesh.</p>
<p class="c0 c1"><span class="c0"><a name="conclusion" id="protocol3"></a><strong> Conclusion and future improvements</strong></span></p>
<p class="c0 c1">I am very happy to have taken SLIP in my fourth year and to have been part of Group C. I really think that the members of my team were devoted, hard working and helping which was key to our successful completion of the project. I can honestly say that SLIP was one of the most challenging and in the same time intriguing courses I have taken so far. SLIP introduced to me the low level programming as well as some hardware and networking knowledge. </p>
<p class="c0 c1">There is of course room for improvement for our project. Some examples for future development could be making a more sophisticated protocol which might support sending audio data through the mesh. Another thing that can be done is to include a GPS feature in the project. A device can frequently send others packets including its location. In some extreme environments this might be useful for a network like this.</p>
</body></html></p></body></html>  </div>
  
  
    <div class="footer">
    <div align="center" class="footer">November 2012.
      COPYRIGHT (C) 2012 GROUP C. ALL RIGHTS RESERVED. </div>
  </div>
  
  
  <!-- end .container --></div>
</body>
</html>
